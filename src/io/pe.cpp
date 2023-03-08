/**
 * Copyright (c) 2023 System233
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#include "msga/io/pe.h"
#include "msga/config.h"
#include "helper.h"

#define RELOC_PAGE_SIZE 0x1000
std::istream &operator>>(std::istream &is, msga::io::pe::section_t &section)
{
    is >> section.meta;
    auto pos = is.tellg();
    is.seekg(section.meta.PointerToRawData);
    section.data.resize(section.meta.SizeOfRawData);
    is >> section.data;
    is.seekg(pos);
    return is;
}
bool msga::io::pe::load(std::istream &is)
{
    is.seekg(std::ios::beg);
    is >> m_dos_h;
    if (m_dos_h.e_magic != 0x5A4D)
    {
        return false;
    }
    auto stub = m_dos_h.e_lfanew - sizeof(m_dos_h);
    m_dos_stub.resize(stub);
    is >> m_dos_stub;
    is >> m_nt_h;
    switch (arch())
    {
    case io::mode::x64:
        is >> m_opt64_h;
        break;
    case io::mode::x32:
        is >> m_opt32_h;
        break;
    default:
        return false;
    }
    for (auto i = 0; i < m_nt_h.FileHeader.NumberOfSections; ++i)
    {
        section_t section;
        is >> section;
        m_sections.emplace_back(std::move(section));
    }
    if (m_nt_h.FileHeader.PointerToSymbolTable)
    {
        is.seekg(m_nt_h.FileHeader.PointerToSymbolTable);
        m_symbol.table.resize(m_nt_h.FileHeader.NumberOfSymbols);
        is >> m_symbol.table;
        uint32_t sym_size;
        is >> raw<uint32_t>(sym_size);
        if (sym_size)
        {
            m_symbol.data.resize(sym_size - sizeof(sym_size));
            is >> m_symbol.data;
        }
    }
    load_relocation_table();
    return true;
}
void msga::io::pe::load_relocation_table()
{
    auto &dir = directory(IMAGE_DIRECTORY_ENTRY_BASERELOC);
    size_t offset = 0;
    auto base = &get<uint8_t>(RVA2VA(dir.VirtualAddress));
    while (offset < dir.Size)
    {
        auto &rel = *reinterpret_cast<IMAGE_BASE_RELOCATION *>(base + offset);
        auto &table = m_rel_table[rel.VirtualAddress];
        auto block = reinterpret_cast<IMAGE_BASE_RELOCATION_BLOCK *>(base + offset + sizeof(IMAGE_BASE_RELOCATION));
        auto len = (rel.SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(IMAGE_BASE_RELOCATION_BLOCK);
        for (auto i = 0; i < len; ++i)
        {
            auto &blk = block[i];
            if (blk.offset == 0 && blk.type == 0)
            {
                continue;
            }
            table[blk.offset] = blk.type;
        }
        offset += rel.SizeOfBlock;
    }
}
msga::io::pe::section_t &msga::io::pe::section(size_t rva)
{
    for (auto &section : m_sections)
    {
        if (section.addr() <= rva && rva < section.addr() + section.size())
        {
            return section;
        }
    }
    throw std::runtime_error("RVA out of range");
}

void msga::io::pe::rebuild_relocation_table()
{
    section_t section;
    auto &data = section.data;
    for (auto &block : m_rel_table)
    {
        auto block_size = block.second.size() * sizeof(IMAGE_BASE_RELOCATION_BLOCK) + sizeof(IMAGE_BASE_RELOCATION);
        IMAGE_BASE_RELOCATION base;
        base.VirtualAddress = block.first;
        base.SizeOfBlock = aligned(block_size, 4);
        data << base;
        for (auto &item : block.second)
        {
            IMAGE_BASE_RELOCATION_BLOCK rel_blcok;
            rel_blcok.offset = item.first;
            rel_blcok.type = item.second;
            data << rel_blcok;
        }
        if (block.second.size() & 1)
        {
            IMAGE_BASE_RELOCATION_BLOCK rel_blcok;
            rel_blcok.offset = 0;
            rel_blcok.type = 0;
            data << rel_blcok;
        }
    }
    // section.resize(aligned(section.size(),option(e_option::SectionAlignment)));
    // section.meta.SizeOfRawData=aligned();
    // section.meta.VirtualAddress=reserved(0)
    auto &dir = directory(IMAGE_DIRECTORY_ENTRY_BASERELOC);
    if (dir.VirtualAddress)
    {
        auto &rel = pe::section(dir.VirtualAddress);
        if (section.size() <= rel.size())
        {
            memcpy(rel.get(), section.get(), section.size());
            dir.Size = section.size();
            return;
        }
    }
    if (section.size())
    {
        auto req_size = section.size();
        auto aligned_size = aligned(section.size(), option(e_option::SectionAlignment));
        section.resize(aligned_size);
        auto rva = reserved(0, section.size());
        section.addr(rva);
        section.name(".reloc");
        dir.Size = req_size;
        dir.VirtualAddress = section.addr();
        m_sections.emplace_back(std::move(section));
    }
}

bool msga::io::pe::dump(std::ostream &os)
{
    rebuild_relocation_table();
    auto header_size = m_dos_h.e_lfanew + sizeof(m_nt_h) + m_nt_h.FileHeader.SizeOfOptionalHeader + m_sections.size() * sizeof(section_t::meta);
    auto base = aligned(header_size, option(e_option::FileAlignment));
    os.seekp(base);
    size_t image_size = 0;
    for (auto &section : m_sections)
    {
        auto current = os.tellp();
        section.meta.PointerToRawData = current;
        section.meta.SizeOfRawData = section.data.size();
        image_size = std::max(image_size, section.addr() + section.size());
        os << section.data;
        auto next = aligned((uint64_t)os.tellp(), option(e_option::FileAlignment));
        os.seekp(next);
    }
    if (!m_symbol.table.empty())
    {
        m_nt_h.FileHeader.PointerToSymbolTable = DWORD(os.tellp());
        m_nt_h.FileHeader.NumberOfSymbols = m_symbol.table.size();
        uint32_t symbol_size = m_symbol.data.size() + sizeof(uint32_t);
        os << m_symbol.table;
        os << raw<uint32_t>(symbol_size);
        os << m_symbol.data;
    }

    m_nt_h.FileHeader.NumberOfSections = m_sections.size();
    option(e_option::SizeOfImage, image_size);
    // m_nt_h.FileHeader.NumberOfSections=m_sections.size();
    option(e_option::CheckSum, 0);
    // option()
    os.seekp(0);
    os << m_dos_h;
    os << m_dos_stub;
    os << m_nt_h;
    switch (arch())
    {
    case io::mode::x64:
        os << m_opt64_h;
        break;
    case io::mode::x32:
        os << m_opt32_h;
        break;
    default:
        return false;
    }
    for (auto &section : m_sections)
    {
        os << section.meta;
    }
    return true;
}

uint64_t msga::io::pe::option64(e_option opt) const
{
#define CASE(X)       \
    case e_option::X: \
        return m_opt64_h.X;
    switch (opt)
    {
        CASE(MajorLinkerVersion);
        CASE(MinorLinkerVersion);
        CASE(SizeOfCode);
        CASE(SizeOfInitializedData);
        CASE(SizeOfUninitializedData);
        CASE(AddressOfEntryPoint);
        CASE(BaseOfCode);
        CASE(ImageBase);
        CASE(SectionAlignment);
        CASE(FileAlignment);
        CASE(MajorOperatingSystemVersion);
        CASE(MinorOperatingSystemVersion);
        CASE(MajorImageVersion);
        CASE(MinorImageVersion);
        CASE(MajorSubsystemVersion);
        CASE(MinorSubsystemVersion);
        CASE(Win32VersionValue);
        CASE(SizeOfImage);
        CASE(SizeOfHeaders);
        CASE(CheckSum);
        CASE(Subsystem);
        CASE(DllCharacteristics);
        CASE(SizeOfStackReserve);
        CASE(SizeOfStackCommit);
        CASE(SizeOfHeapReserve);
        CASE(SizeOfHeapCommit);
        CASE(LoaderFlags);
        CASE(NumberOfRvaAndSizes);
    default:
        throw std::runtime_error("bad option type");
    }
#undef CASE
}
void msga::io::pe::option64(e_option opt, uint64_t value)
{
#define CASE(X)              \
    case e_option::X:        \
        m_opt64_h.X = value; \
        break;
    switch (opt)
    {
        CASE(MajorLinkerVersion);
        CASE(MinorLinkerVersion);
        CASE(SizeOfCode);
        CASE(SizeOfInitializedData);
        CASE(SizeOfUninitializedData);
        CASE(AddressOfEntryPoint);
        CASE(BaseOfCode);
        CASE(ImageBase);
        CASE(SectionAlignment);
        CASE(FileAlignment);
        CASE(MajorOperatingSystemVersion);
        CASE(MinorOperatingSystemVersion);
        CASE(MajorImageVersion);
        CASE(MinorImageVersion);
        CASE(MajorSubsystemVersion);
        CASE(MinorSubsystemVersion);
        CASE(Win32VersionValue);
        CASE(SizeOfImage);
        CASE(SizeOfHeaders);
        CASE(CheckSum);
        CASE(Subsystem);
        CASE(DllCharacteristics);
        CASE(SizeOfStackReserve);
        CASE(SizeOfStackCommit);
        CASE(SizeOfHeapReserve);
        CASE(SizeOfHeapCommit);
        CASE(LoaderFlags);
        CASE(NumberOfRvaAndSizes);
    default:
        throw std::runtime_error("bad option type");
    }
#undef CASE
}
uint64_t msga::io::pe::option32(e_option opt) const
{
#define CASE(X)       \
    case e_option::X: \
        return m_opt32_h.X;
    switch (opt)
    {
        CASE(MajorLinkerVersion);
        CASE(MinorLinkerVersion);
        CASE(SizeOfCode);
        CASE(SizeOfInitializedData);
        CASE(SizeOfUninitializedData);
        CASE(AddressOfEntryPoint);
        CASE(BaseOfCode);
        CASE(BaseOfData);
        CASE(ImageBase);
        CASE(SectionAlignment);
        CASE(FileAlignment);
        CASE(MajorOperatingSystemVersion);
        CASE(MinorOperatingSystemVersion);
        CASE(MajorImageVersion);
        CASE(MinorImageVersion);
        CASE(MajorSubsystemVersion);
        CASE(MinorSubsystemVersion);
        CASE(Win32VersionValue);
        CASE(SizeOfImage);
        CASE(SizeOfHeaders);
        CASE(CheckSum);
        CASE(Subsystem);
        CASE(DllCharacteristics);
        CASE(SizeOfStackReserve);
        CASE(SizeOfStackCommit);
        CASE(SizeOfHeapReserve);
        CASE(SizeOfHeapCommit);
        CASE(LoaderFlags);
        CASE(NumberOfRvaAndSizes);
    default:
        throw std::runtime_error("bad option type");
    }
#undef CASE
}
void msga::io::pe::option32(e_option opt, uint64_t value)
{
#define CASE(X)              \
    case e_option::X:        \
        m_opt32_h.X = value; \
        break;
    switch (opt)
    {
        CASE(MajorLinkerVersion);
        CASE(MinorLinkerVersion);
        CASE(SizeOfCode);
        CASE(SizeOfInitializedData);
        CASE(SizeOfUninitializedData);
        CASE(AddressOfEntryPoint);
        CASE(BaseOfCode);
        CASE(BaseOfData);
        CASE(ImageBase);
        CASE(SectionAlignment);
        CASE(FileAlignment);
        CASE(MajorOperatingSystemVersion);
        CASE(MinorOperatingSystemVersion);
        CASE(MajorImageVersion);
        CASE(MinorImageVersion);
        CASE(MajorSubsystemVersion);
        CASE(MinorSubsystemVersion);
        CASE(Win32VersionValue);
        CASE(SizeOfImage);
        CASE(SizeOfHeaders);
        CASE(CheckSum);
        CASE(Subsystem);
        CASE(DllCharacteristics);
        CASE(SizeOfStackReserve);
        CASE(SizeOfStackCommit);
        CASE(SizeOfHeapReserve);
        CASE(SizeOfHeapCommit);
        CASE(LoaderFlags);
        CASE(NumberOfRvaAndSizes);
    default:
        throw std::runtime_error("bad option type");
    }
#undef CASE
}

msga::addr_t msga::io::pe::reserved(addr_t preferred, size_t len)
{
    len = aligned(len, option(e_option::FileAlignment));
    std::map<addr_t, size_t> map;
    for (auto &section : m_sections)
    {
        map.emplace(section.addr(), section.size());
    }
    preferred = aligned(preferred, option(e_option::SectionAlignment));
    if (!preferred)
    {
        preferred = option(e_option::SectionAlignment);
    }
    addr_t last_end = 0;
    addr_t best_dist = preferred - last_end;
    addr_t best_end = last_end;
    for (auto &item : map)
    {
        addr_t cur_end = item.first + item.second;
        if (cur_end - last_end < len)
        {
            continue;
        }
        auto cur_dist = preferred - last_end;
        if (cur_dist < best_dist)
        {
            best_end = cur_end;
            best_dist = cur_dist;
        }
        last_end = cur_end;
    }
    if (best_end == 0)
    {
        best_end = last_end ? last_end : preferred;
    }
    return aligned(best_end, option(e_option::SectionAlignment));
}

msga::addr_t msga::io::pe::alloc(size_t size, addr_t preferred)
{
    if (!preferred)
    {
        preferred = VA2RVA(preferred);
    }
    for (auto &section : m_sections)
    {
        if (memcmp(section.meta.Name, MSGA_SECTION_NAME, sizeof(MSGA_SECTION_NAME)))
        {
            continue;
        }
        auto addr = (uint8_t *)allocate(section.data.data(), section.data.size(), size, static_cast<size_t>(arch()));
        if (addr)
        {
            return RVA2VA(section.addr() + (addr - section.get()));
        }
    }
    auto section_size = aligned(size, option(e_option::FileAlignment));
    section_t section;
    memcpy(section.meta.Name, MSGA_SECTION_NAME, sizeof(MSGA_SECTION_NAME));
    section.meta.VirtualAddress = reserved(preferred, section_size);
    section.meta.Misc.VirtualSize = section_size;
    section.meta.Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_EXECUTE;
    section.meta.SizeOfRawData = section_size;
    section.data.resize(section_size);
    auto alloced = (uint8_t *)allocate(section.data.data(), section.data.size(), size, static_cast<size_t>(arch()));
    auto offset = alloced - section.data.data();
    auto rva = section.addr() + offset;
    m_sections.emplace_back(std::move(section));
    return RVA2VA(rva);
}
void msga::io::pe::free(addr_t addr,size_t len)
{
    deallocate((void *)&get<uint8_t>(addr));
}
void msga::io::pe::rebase(addr_t addr, rel_base_t*opt)
{
    addr = VA2RVA(addr+opt->offset());
    auto base = floor(addr, RELOC_PAGE_SIZE);
    auto offset = addr - base;
    auto type=0;
    switch (opt->type())
    {
    case e_rel::e_rela:
        type = arch()==io::mode::x64?IMAGE_REL_BASED_DIR64:IMAGE_REL_BASED_HIGHLOW;
        break;
    case e_rel::e_rel16:
        type = IMAGE_REL_BASED_LOW;
        break;
    case e_rel::e_rel32:
        type = IMAGE_REL_BASED_HIGHLOW;
        break;
    case e_rel::e_rel64:
        type = IMAGE_REL_BASED_DIR64;
        break;
    case e_rel::e_rel_sys:
        type = dynamic_cast<rel_pe_t*>(opt)->data();
    default:
        throw std::runtime_error("bad e_rel type");
    }
    m_rel_table[base][offset] = type;
}
void msga::io::pe::debase(addr_t addr)
{
    addr = VA2RVA(addr);
    auto base = floor(addr, RELOC_PAGE_SIZE);
    auto offset = addr - base;
    if (m_rel_table.count(base))
    {
        auto &block = m_rel_table[base];
        block.erase(offset);
        if (block.empty())
        {
            m_rel_table.erase(base);
        }
    }
}
void msga::io::pe::search_rebase(rel_list_t &item, addr_t addr, size_t len)
{
    auto rva = VA2RVA(addr);
    auto base = floor(rva, RELOC_PAGE_SIZE);
    auto end = rva + len;
    for (auto i = 0; i < len; i += RELOC_PAGE_SIZE)
    {
        auto cur_base = base + i;
        if (m_rel_table.count(cur_base))
        {
            auto &block = m_rel_table[cur_base];
            for (auto &blk : block)
            {
                auto cur_addr = cur_base + blk.first;
                if (cur_addr >= rva && cur_addr < end)
                {
                    item.emplace_back(std::make_unique<rel_pe_t>(cur_addr - rva,blk.second));
                }
            }
        }
    }
}

void msga::io::pe::range_rebase(rel_list_t const &items, addr_t addr, size_t len)
{
    auto rva = VA2RVA(addr);
    auto base = floor(rva, RELOC_PAGE_SIZE);
    auto end = rva + len;
    for (auto i = 0; i < len; i += RELOC_PAGE_SIZE)
    {
        auto cur_base = base + i;
        if (m_rel_table.count(cur_base))
        {
            auto &block = m_rel_table[cur_base];
            auto it = block.begin();
            while (it != block.end())
            {
                auto cur_addr = cur_base + it->first;
                if (cur_addr >= rva && cur_addr < end)
                {
                    it = block.erase(it);
                }
                else
                {
                    it++;
                }
            }
            if (block.empty())
            {
                m_rel_table.erase(cur_base);
            }
        }
    }
    for (auto&item : items)
    {
        rebase(addr,item.get());
    }
}
void msga::io::pe::read(void *data, addr_t addr, size_t len)
{
    memcpy(data, &get<uint8_t>(addr), len);
}
void msga::io::pe::write(void const *data, addr_t addr, size_t len)
{
    memcpy(&get<uint8_t>(addr), data, len);
}

bool msga::io::pe::load(std::string const &path)
{
    std::ifstream is(path, std::ios::binary);
    if (!is)
    {
        return false;
    }
    return load(is);
}
bool msga::io::pe::dump(std::string const &path)
{
    std::ofstream os(path, std::ios::binary);
    if (!os)
    {
        return false;
    }
    return dump(os);
}

void msga::io::pe::section_t::name(char const *name)
{
    memset(meta.Name, 0, sizeof(meta.Name));
    memcpy(meta.Name, name, std::min(sizeof(meta.Name), strnlen(name, sizeof(sizeof(meta.Name)))));
}
void *msga::io::pe::get(addr_t va)
{
    auto rva=VA2RVA(va);
    for (auto &section : m_sections)
    {
        auto base = section.meta.VirtualAddress;
        auto vsize = section.meta.Misc.VirtualSize;
        if (base <= rva && rva < base + vsize)
        {
            if(section.size()<vsize){
                // IF bss
                section.resize(aligned(vsize,option(e_option::FileAlignment)));
            }
            return &section.data[rva - base];
        }
    }
    throw std::runtime_error("address out of range");
}
msga::pe::IMAGE_DATA_DIRECTORY &msga::io::pe::directory(int entry)
{
    switch (arch())
    {
    case io::mode::x64:
        return m_opt64_h.DataDirectory[entry];
    case io::mode::x32:
        return m_opt32_h.DataDirectory[entry];
    default:
        throw std::runtime_error("bad arch");
    }
};