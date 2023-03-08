// Copyright (c) 2023 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "msga/io.h"
#include <istream>
#include <ostream>
#include <list>
#include <map>
#include <memory>
#include <algorithm>

// #include <elf.h>
#define ELFMAG "\177ELF"
#define ELF32_R_SYM(x) ((x) >> 8)
#define ELF32_R_TYPE(x) ((x)&0xff)
#define ELF32_R_INFO(sym, type) ((((Elf32_Word)sym) << 8) | ((type) & 0xff))

#define ELF64_R_SYM(i) ((i) >> 32)
#define ELF64_R_TYPE(i) ((i)&0xffffffff)
#define ELF64_R_INFO(sym, type) ((((Elf64_Xword)sym) << 32) | ((type) & 0xffffffff))


#define ELF_ST_TYPE(x) ((x) & 0xf)
#define ELF_ST_INFO(x,t) (((x) << 4) + ((t) & 0xf))
#define ELF32_ST_INFO(x,t) ELF_ST_INFO(x,t)
#define ELF64_ST_INFO(x,t) ELF_ST_INFO(x,t)


namespace msga
{
    namespace elf
    {
        using Elf32_Byte = uint8_t;
        using Elf32_Sbyte = int8_t;

        using Elf32_Addr = uint32_t;
        using Elf32_Half = uint16_t;
        using Elf32_Off = uint32_t;
        using Elf32_Sword = int32_t;
        using Elf32_Word = uint32_t;

        using Elf64_Addr = uint64_t;
        using Elf64_Half = uint16_t;
        using Elf64_SHalf = int16_t;
        using Elf64_Off = uint64_t;
        using Elf64_Sword = int32_t;
        using Elf64_Word = uint32_t;
        using Elf64_Xword = uint64_t;
        using Elf64_Sxword = int64_t;
        enum Elf_EM
        {
            EM_386 = 3,
            EM_ARM = 40,
            EM_X86_64 = 62,
            EM_AARCH64 = 183
        };
        enum Elf_PF
        {
            PF_X = 0x1,
            PF_W = 0x2,
            PF_R = 0x4,
        };
        enum Elf_R_386
        {
            R_386_NONE = 0,
            R_386_32 = 1,
            R_386_PC32 = 2,
            R_386_GOT32 = 3,
            R_386_PLT32 = 4,
            R_386_COPY = 5,
            R_386_GLOB_DAT = 6,
            R_386_JMP_SLOT = 7,
            R_386_RELATIVE = 8,
            R_386_GOTOFF = 9,
            R_386_GOTPC = 10,
            R_386_32PLT = 11,
            R_386_16 = 20,
            R_386_PC16 = 21,
            R_386_8 = 22,
            R_386_PC8 = 23,
            R_386_SIZE32 = 38,
        };
        enum Elf_R_AMD64
        {
            R_AMD64_NONE = 0,
            R_AMD64_64 = 1,
            R_AMD64_PC32 = 2,
            R_AMD64_GOT32 = 3,
            R_AMD64_PLT32 = 4,
            R_AMD64_COPY = 5,
            R_AMD64_GLOB_DAT = 6,
            R_AMD64_JUMP_SLOT = 7,
            R_AMD64_RELATIVE = 8,
            R_AMD64_GOTPCREL = 9,
            R_AMD64_32 = 10,
            R_AMD64_32S = 11,
            R_AMD64_16 = 12,
            R_AMD64_PC16 = 13,
            R_AMD64_8 = 14,
            R_AMD64_PC8 = 15,
            R_AMD64_PC64 = 24,
            R_AMD64_GOTOFF64 = 25,
            R_AMD64_GOTPC32 = 26,
            R_AMD64_SIZE32 = 32,
            R_AMD64_SIZE64 = 33,

        };
        enum Elf_PT
        {
            PT_NULL = 0,
            PT_LOAD = 1,
            PT_DYNAMIC = 2,
            PT_INTERP = 3,
            PT_NOTE = 4,
            PT_SHLIB = 5,
            PT_PHDR = 6,
            PT_TLS = 7,
            PT_LOOS = 0x60000000,
            PT_HIOS = 0x6fffffff,
            PT_LOPROC = 0x70000000,
            PT_HIPROC = 0x7fffffff,
            PT_GNU_EH_FRAME = 0x6474e550,
            PT_GNU_PROPERTY = 0x6474e553,
            PT_GNU_STACK = (PT_LOOS + 0x474e551)
        };
        enum Elf_SHT
        {
            SHT_NULL = 0,
            SHT_PROGBITS = 1,
            SHT_SYMTAB = 2,
            SHT_STRTAB = 3,
            SHT_RELA = 4,
            SHT_HASH = 5,
            SHT_DYNAMIC = 6,
            SHT_NOTE = 7,
            SHT_NOBITS = 8,
            SHT_REL = 9,
            SHT_SHLIB = 10,
            SHT_DYNSYM = 11,
            SHT_NUM = 12,
            SHT_LOPROC = 0x70000000,
            SHT_HIPROC = 0x7fffffff,
            SHT_LOUSER = 0x80000000,
            SHT_HIUSER = 0xffffffff,
        };
        enum Elf_DT
        {
            DT_NULL = 0,
            DT_NEEDED = 1,
            DT_PLTRELSZ = 2,
            DT_PLTGOT = 3,
            DT_HASH = 4,
            DT_STRTAB = 5,
            DT_SYMTAB = 6,
            DT_RELA = 7,
            DT_RELASZ = 8,
            DT_RELAENT = 9,
            DT_STRSZ = 10,
            DT_SYMENT = 11,
            DT_INIT = 12,
            DT_FINI = 13,
            DT_SONAME = 14,
            DT_RPATH = 15,
            DT_SYMBOLIC = 16,
            DT_REL = 17,
            DT_RELSZ = 18,
            DT_RELENT = 19,
            DT_PLTREL = 20,
            DT_DEBUG = 21,
            DT_TEXTREL = 22,
            DT_JMPREL = 23,
            DT_ENCODING = 32,
            DT_LOOS = 0x6000000d,
            DT_HIOS = 0x6ffff000,
            DT_VALRNGLO = 0x6ffffd00,
            DT_VALRNGHI = 0x6ffffdff,
            DT_ADDRRNGLO = 0x6ffffe00,
            DT_ADDRRNGHI = 0x6ffffeff,
            DT_VERSYM = 0x6ffffff0,
            DT_RELACOUNT = 0x6ffffff9,
            DT_RELCOUNT = 0x6ffffffa,
            DT_FLAGS_1 = 0x6ffffffb,
            DT_VERDEF = 0x6ffffffc,
            DT_VERDEFNUM = 0x6ffffffd,
            DT_VERNEED = 0x6ffffffe,
            DT_VERNEEDNUM = 0x6fffffff,
            DT_LOPROC = 0x70000000,
            DT_HIPROC = 0x7fffffff,
        };
        enum Elf_SHF
        {
            SHF_WRITE = 0x1,
            SHF_ALLOC = 0x2,
            SHF_EXECINSTR = 0x4,
            SHF_RELA_LIVEPATCH = 0x00100000,
            SHF_RO_AFTER_INIT = 0x00200000,
            SHF_MASKPROC = 0xf0000000,
        };
        enum Elf_SHN
        {
            SHN_UNDEF = 0,
            SHN_LORESERVE = 0xff00,
            SHN_LOPROC = 0xff00,
            SHN_HIPROC = 0xff1f,
            SHN_LIVEPATCH = 0xff20,
            SHN_ABS = 0xfff1,
            SHN_COMMON = 0xfff2,
            SHN_HIRESERVE = 0xffff,
        };
        enum Elf_Class : uint8_t
        {
            EC_None,
            EC_ELF32,
            EC_ELF64,
            EC_NUM
        };
        struct Elf_Ident
        {
            char e_magic[4];
            Elf_Class e_class;
            uint8_t e_data;
            uint8_t e_version;
            uint8_t e_osabi;
            uint8_t e_pad[8];
        };
        struct Elf32_Ehdr
        {
            Elf32_Half e_type;
            Elf32_Half e_machine;
            Elf32_Word e_version;
            Elf32_Addr e_entry;
            Elf32_Off e_phoff;
            Elf32_Off e_shoff;
            Elf32_Word e_flags;
            Elf32_Half e_ehsize;
            Elf32_Half e_phentsize;
            Elf32_Half e_phnum;
            Elf32_Half e_shentsize;
            Elf32_Half e_shnum;
            Elf32_Half e_shstrndx;
        };
        struct Elf64_Ehdr
        {
            Elf64_Half e_type;
            Elf64_Half e_machine;
            Elf64_Word e_version;
            Elf64_Addr e_entry;
            Elf64_Off e_phoff;
            Elf64_Off e_shoff;
            Elf64_Word e_flags;
            Elf64_Half e_ehsize;
            Elf64_Half e_phentsize;
            Elf64_Half e_phnum;
            Elf64_Half e_shentsize;
            Elf64_Half e_shnum;
            Elf64_Half e_shstrndx;
        };
        struct Elf32_Phdr
        {
            Elf32_Word p_type;
            Elf32_Off p_offset;
            Elf32_Addr p_vaddr;
            Elf32_Addr p_paddr;
            Elf32_Word p_filesz;
            Elf32_Word p_memsz;
            Elf32_Word p_flags;
            Elf32_Word p_align;
        };
        struct Elf64_Phdr
        {
            Elf64_Word p_type;
            Elf64_Word p_flags;
            Elf64_Off p_offset;
            Elf64_Addr p_vaddr;
            Elf64_Addr p_paddr;
            Elf64_Xword p_filesz;
            Elf64_Xword p_memsz;
            Elf64_Xword p_align;
        };
        struct Elf32_Shdr
        {
            Elf32_Word sh_name;
            Elf32_Word sh_type;
            Elf32_Word sh_flags;
            Elf32_Addr sh_addr;
            Elf32_Off sh_offset;
            Elf32_Word sh_size;
            Elf32_Word sh_link;
            Elf32_Word sh_info;
            Elf32_Word sh_addralign;
            Elf32_Word sh_entsize;
        };
        struct Elf64_Shdr
        {
            Elf64_Word sh_name;
            Elf64_Word sh_type;
            Elf64_Xword sh_flags;
            Elf64_Addr sh_addr;
            Elf64_Off sh_offset;
            Elf64_Xword sh_size;
            Elf64_Word sh_link;
            Elf64_Word sh_info;
            Elf64_Xword sh_addralign;
            Elf64_Xword sh_entsize;
        };
        struct Elf32_Dyn
        {
            Elf32_Sword d_tag;
            union
            {
                Elf32_Sword d_val;
                Elf32_Addr d_ptr;
            } d_un;
        };
        struct Elf64_Dyn
        {
            Elf64_Sxword d_tag;
            union
            {
                Elf64_Xword d_val;
                Elf64_Addr d_ptr;
            } d_un;
        };
        struct Elf32_Rel
        {
            Elf32_Addr r_offset;
            Elf32_Word r_info;
        };
        struct Elf64_Rel
        {
            Elf64_Addr r_offset;
            Elf64_Xword r_info;
        };
        struct Elf32_Rela
        {
            Elf32_Addr r_offset;
            Elf32_Word r_info;
            Elf32_Sword r_addend;
        };
        struct Elf64_Rela
        {
            Elf64_Addr r_offset;
            Elf64_Xword r_info;
            Elf64_Sxword r_addend;
        };
        struct elf32{
            enum{
                e_class=Elf_Class::EC_ELF32
            };
            using e_rel=Elf_R_386;
            using ehdr_t=Elf32_Ehdr;
            using shdr_t=Elf32_Shdr;
            using phdr_t=Elf32_Phdr;
            using dyn_t=Elf32_Dyn;
            using rel_t=Elf32_Rel;
            using rela_t=Elf32_Rela;
        };
        
        struct elf64{
            enum{
                e_class=Elf_Class::EC_ELF64
            };
            using e_rel=Elf_R_AMD64;
            using ehdr_t=Elf64_Ehdr;
            using shdr_t=Elf64_Shdr;
            using phdr_t=Elf64_Phdr;
            using dyn_t=Elf64_Dyn;
            using rel_t=Elf64_Rel;
            using rela_t=Elf64_Rela;
        };
    }
    namespace io
    {
        using namespace msga::elf;
        class elf_base : public io::base{
        public:
            virtual io::mode arch() const override=0;
            virtual io::mode mode() const override=0;
            virtual void read(void *data, addr_t addr, size_t len)override=0;
            virtual void write(void const *data, addr_t addr, size_t len)override=0;
            virtual void rebase(addr_t addr, rel_base_t*opt)override=0;
            virtual void debase(addr_t addr)override=0;
            virtual void search_rebase(rel_list_t &items, addr_t addr, size_t len)override=0;
            virtual void range_rebase(rel_list_t const &items, addr_t addr, size_t len)override=0;
            virtual addr_t alloc(size_t size, addr_t preferred = 0)override=0;
            virtual void free(addr_t addr,size_t len)override=0;
            virtual bool load(std::istream &is)=0;
            virtual bool load(std::string const&filename);
            virtual bool dump(std::ostream &os)=0;
            virtual bool dump(std::string const&filename);
            virtual void *get(addr_t va)=0;
            template <class T>
            T &get(addr_t va)
            {
                return *reinterpret_cast<T *>(get(va));
            }
            template <class T>
            void set(addr_t va, T &&data)
            {
                *reinterpret_cast<T *>(get(va)) = data;
            }
        };
        template<class elf_type>
        class elf_impl : public elf_base
        {
        public:
            struct relx_t
            {
                using info_t=decltype(elf_type::rela_t::r_info);
                using addend_t=decltype(elf_type::rela_t::r_addend);
                bool is_rela;
                info_t r_info;
                addend_t r_addend;
                relx_t():is_rela(false),r_info(0),r_addend(0){}
                relx_t(info_t info):is_rela(false),r_info(info),r_addend(0){};
                relx_t(info_t info,addend_t addend):is_rela(true),r_info(info),r_addend(addend){};
            };
            using rel_elf_t=rel_sys_t<relx_t>;
            struct section_t
            {
                typename elf_type::shdr_t shdr;
                std::vector<uint8_t> data;
                std::weak_ptr<section_t>link;
            };

            struct program_t
            {
                typename elf_type::phdr_t phdr;
                std::list<std::weak_ptr<section_t>> sections;
            };
            struct section_option_t
            {
                addr_t perferred;
                size_t size;
                Elf_SHT sh_type;
                size_t sh_align;
                Elf_SHF sh_flag;
                Elf_PT p_type;
                Elf_PF p_flag;
                size_t p_align;
            };
            
        private:
            Elf_Ident m_ident;
            typename elf_type::ehdr_t m_ehdr_h;
            std::list<program_t> m_programs;
            std::vector<std::shared_ptr<section_t>> m_sections;
            std::map<section_t*,std::map<addr_t,relx_t>>m_rel_table;
            std::weak_ptr<section_t>m_shstrtab_ref;
            std::weak_ptr<section_t>m_dynamic_ref;
            section_t&shstrtab();
            section_t*section(std::string const&name,section_t*start=nullptr);
            typename elf_type::dyn_t*dynamic(Elf_DT dt);
            addr_t reserved(addr_t perferred, size_t len,size_t align=1);

            std::shared_ptr<section_t> make_section(std::string const&name,section_option_t const&option);
            section_t&section(addr_t va);
            void load_relocation_table();
            void rebuild_relocation_table();
        public:
            void debug();
            virtual io::mode arch() const override;
            virtual io::mode mode() const override{ return io::mode::x32; };
            virtual void read(void *data, addr_t addr, size_t len)override
            {
                auto base= static_cast<uint8_t const*>(get(addr));
                std::copy(base,base+len, static_cast<uint8_t*>(data));
            }
            virtual void write(void const *data, addr_t addr, size_t len)override
            {
                auto base= static_cast<uint8_t const*>(data);
                std::copy(base,base+len, static_cast<uint8_t*>(get(addr)));
            }
            virtual void rebase(addr_t addr, rel_base_t*opt)override;
            virtual void debase(addr_t addr)override;
            virtual void search_rebase(rel_list_t &items, addr_t addr, size_t len)override;
            virtual void range_rebase(rel_list_t const &items, addr_t addr, size_t len)override;
            virtual addr_t alloc(size_t size, addr_t preferred = 0)override;
            virtual void free(addr_t addr,size_t len)override;
            bool load(std::istream &is)override;
            bool dump(std::ostream &os)override;
            void *get(addr_t va)override;
            template <class T>
            T &get(addr_t va)
            {
                return *reinterpret_cast<T *>(get(va));
            }
            template <class T>
            void set(addr_t va, T &&data)
            {
                *reinterpret_cast<T *>(get(va)) = data;
            }
        };
        class elf: public elf_base{
            std::shared_ptr<elf_base>m_impl;
            public:
            virtual io::mode arch() const override{return m_impl->arch();};
            virtual io::mode mode() const override{return m_impl->mode();};
            virtual void read(void *data, addr_t addr, size_t len)override
            {
                return m_impl->read(data,addr,len);
            }
            virtual void write(void const *data, addr_t addr, size_t len)override
            {
                return m_impl->write(data,addr,len);
            }
            virtual void rebase(addr_t addr, rel_base_t*opt)override{m_impl->rebase(addr,opt);};;
            virtual void debase(addr_t addr)override{m_impl->debase(addr);};;
            virtual void search_rebase(rel_list_t &items, addr_t addr, size_t len)override{m_impl->search_rebase(items,addr,len);};;
            virtual void range_rebase(rel_list_t const &items, addr_t addr, size_t len)override{m_impl->range_rebase(items,addr,len);};
            virtual addr_t alloc(size_t size, addr_t preferred = 0)override{return m_impl->alloc(size,preferred);};
            virtual void free(addr_t addr,size_t len)override{m_impl->free(addr,len);};
            virtual bool load(std::istream &is)override;
            virtual bool dump(std::ostream &os)override{return m_impl->dump(os);};
            virtual bool load(std::string const&filename)override{return elf_base::load(filename);};
            virtual bool dump(std::string const&filename)override{return elf_base::dump(filename);};
            virtual void *get(addr_t va)override{return m_impl->get(va);};
        };

    }
}