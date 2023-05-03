// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include <fstream>

#include "msga/config.h"
#include "msga/io/elf.h"
#include <stdio.h>
#define MSGA_ELF_PAGE_SIZE 0x1000
// #include "elf.h"
// #include "elf_debug.cpp"
#include "helper.h"
#include <cstring>
using namespace msga;

template<class elf_type>
io::mode msga::io::elf_impl<elf_type>::arch()const{
    switch (m_ident.e_class)
    {
    case Elf_Class::EC_ELF64:return io::mode::x64;
    case Elf_Class::EC_ELF32:return io::mode::x32;
    default:
        break;
    }
    throw std::runtime_error("bad elf class");
}

template<class elf_type>
bool msga::io::elf_impl<elf_type>::load(std::istream &is)
{
    is.seekg(std::ios::beg);
    is>>m_ident;
    if(std::memcmp(m_ident.e_magic,ELFMAG,sizeof(ELFMAG)-1)){
        return false;
    }
    if(m_ident.e_class!=elf_type::e_class){
        return false;
    }
    is>>m_ehdr_h;
    is.seekg(m_ehdr_h.e_phoff);
    for(auto i=0;i<m_ehdr_h.e_phnum;++i){
        program_t program;
        is>>program.phdr;
        if(program.phdr.p_type==Elf_PT::PT_PHDR){
            continue;
        }
        m_programs.emplace_back(std::move(program));
    }
    is.seekg(m_ehdr_h.e_shoff);
    for(auto i=0;i<m_ehdr_h.e_shnum;++i){
        auto section=std::make_shared<section_t>();
        is>>section->shdr;
        if(section->shdr.sh_type!=Elf_SHT::SHT_NOBITS){
            auto current=is.tellg();
            is.seekg(section->shdr.sh_offset);
            section->data.resize(section->shdr.sh_size);
            is>>section->data;
            is.seekg(current);
        }
        for(auto&prog:m_programs){
            auto phstart=prog.phdr.p_offset;
            auto shstart=section->shdr.sh_offset;
            if(shstart<phstart){
                continue;
            }
            auto phend=prog.phdr.p_offset+prog.phdr.p_filesz;
            auto shend=section->shdr.sh_offset+section->shdr.sh_size;
            if(shend>phend){
                continue;
            }
            prog.sections.emplace_back(section);
        }
        m_sections.emplace_back(section);

        if(section->shdr.sh_type==Elf_SHT::SHT_DYNAMIC){
            m_dynamic_ref=section;
        }
    }
    for(auto&section:m_sections){
        auto index=section->shdr.sh_link;
        if(index>0&&index<m_sections.size()){
            section->link=m_sections[section->shdr.sh_link];
        }
    }
    if(m_ehdr_h.e_shstrndx<m_sections.size()){
        m_shstrtab_ref=m_sections[m_ehdr_h.e_shstrndx];
    }

    load_relocation_table();
    return true;
}
template<class elf_type>
void* msga::io::elf_impl<elf_type>::get(addr_t addr){
    for(auto&section:m_sections){
        auto shaddr=section->shdr.sh_addr;
        if(section->shdr.sh_addr>addr){
            continue;
        }
        auto shend=section->shdr.sh_addr+section->shdr.sh_size;
        if(addr>=shend){
            continue;
        }
        if(section->shdr.sh_type==Elf_SHT::SHT_NOBITS){
            section->shdr.sh_type=Elf_SHT::SHT_PROGBITS;
            section->data.resize(section->shdr.sh_size);

            program_t*p_prog=nullptr;
            for(auto&prog:m_programs){
                auto phstart=prog.phdr.p_offset;
                auto shstart=section->shdr.sh_offset;
                if(shstart<phstart){
                    continue;
                }
                auto phend=prog.phdr.p_offset+prog.phdr.p_filesz;
                auto shend=section->shdr.sh_offset+section->shdr.sh_size;
                if(shend>phend){
                    continue;
                }
                p_prog=&prog;
                break;
            }
            if(!p_prog){
                program_t prog;
                prog.phdr.p_type=Elf_PT::PT_LOAD;
                prog.phdr.p_flags=Elf_PF::PF_R|Elf_PF::PF_W;
                prog.phdr.p_offset=section->shdr.sh_offset;
                prog.phdr.p_vaddr=section->shdr.sh_addr;
                prog.phdr.p_paddr=section->shdr.sh_addr;
                prog.phdr.p_filesz=section->shdr.sh_size;
                prog.phdr.p_memsz=section->shdr.sh_size;
                prog.phdr.p_align=MSGA_ELF_PAGE_SIZE;
                prog.sections.emplace_back(section);
                m_programs.emplace_back(std::move(prog));
            }
        }
        return &section->data[addr-shaddr];
    }
    throw std::runtime_error("address out of range");
}

template<class elf_type>
void msga::io::elf_impl<elf_type>::load_relocation_table(){
    for(auto&section:m_sections){
        if(section->shdr.sh_type==Elf_SHT::SHT_REL){
            auto relp=reinterpret_cast<typename elf_type::rel_t*>(section->data.data());
            auto num=section->shdr.sh_size/sizeof(*relp);
            auto&map=m_rel_table[section.get()];
            for(auto i=0;i<num;++i){
                auto&rel=relp[i];
                map[rel.r_offset]=relx_t(rel.r_info);
            }
        }
        if(section->shdr.sh_type==Elf_SHT::SHT_RELA){
            auto relp=reinterpret_cast<typename elf_type::rela_t*>(section->data.data());
            auto num=section->shdr.sh_size/sizeof(*relp);
            auto&map=m_rel_table[section.get()];
            for(auto i=0;i<num;++i){
                auto&rel=relp[i];
                map[rel.r_offset]=relx_t(rel.r_info,rel.r_addend);
            }
        }
    }
}
template<class elf_type>
typename elf_type::dyn_t*msga::io::elf_impl<elf_type>::dynamic(Elf_DT dt){
    if(auto dynamic=m_dynamic_ref.lock()){
        typename elf_type::dyn_t*dyn=reinterpret_cast<typename elf_type::dyn_t*>(dynamic->data.data());
        for(auto i=0;i<dynamic->data.size()/dynamic->shdr.sh_entsize;++i){
            if(dyn[i].d_tag==dt){
                return &dyn[i];
            }
        }
    }
    return nullptr;
}
template<class elf_type>
void msga::io::elf_impl<elf_type>::rebuild_relocation_table(){
    for(auto&section_rel:m_rel_table){
        std::vector<uint8_t>data;
        auto&section=section_rel.first;
        if(section->shdr.sh_type==Elf_SHT::SHT_RELA){
            for(auto&item:section_rel.second){
                typename elf_type::rela_t rel;
                rel.r_offset=item.first;
                rel.r_info=item.second.r_info;
                rel.r_addend=item.second.r_addend;
                data<<rel;
            };
        }else if(section->shdr.sh_type==Elf_SHT::SHT_REL){
            for(auto&item:section_rel.second){
                typename elf_type::rel_t rel;
                rel.r_offset=item.first;
                rel.r_info=item.second.r_info;
                data<<rel;
            };
        }else{
            throw std::runtime_error("rebuild_relocation_table(): bad section type");
        }
        if(section->shdr.sh_size<data.size()){
            auto new_addr=reserved(section->shdr.sh_addr,data.size(),section->shdr.sh_addralign);
            auto old_addr=section->shdr.sh_addr;
            if(auto rel=dynamic(Elf_DT::DT_RELA)){
                if(rel->d_un.d_ptr==old_addr){
                    rel->d_un.d_ptr=new_addr;
                    dynamic(Elf_DT::DT_RELASZ)->d_un.d_val=data.size();
                }
            }
            if(auto rel=dynamic(Elf_DT::DT_REL)){
                if(rel->d_un.d_ptr==old_addr){
                    rel->d_un.d_ptr=new_addr;
                    dynamic(Elf_DT::DT_RELSZ)->d_un.d_val=data.size();
                }
            }
            if(auto rel=dynamic(Elf_DT::DT_JMPREL)){
                if(rel->d_un.d_ptr==old_addr){
                    rel->d_un.d_ptr=new_addr;
                    dynamic(Elf_DT::DT_PLTRELSZ)->d_un.d_val=data.size();
                }
            }

            
            section->shdr.sh_addr=new_addr;
            section->shdr.sh_size=data.size();
        }
        section->data=data;
    }
    if(auto rel=dynamic(Elf_DT::DT_RELCOUNT)){
        rel->d_un.d_val=0;
    }
    if(auto rel=dynamic(Elf_DT::DT_RELACOUNT)){
        rel->d_un.d_val=0;
    }
}
/*
    NOTE: 
    PT_PHDR 必须在任何PT_LOAD之前
    PT_LOAD 不应重叠
    PT_LOAD 应以p_vaddr 升序 (elf.h)
    DT_RELCOUNT     指定连续的相对重定位Rel的数量
    DT_RELACOUNT    指定连续的相对重定位Rela的数量
*/
template<class elf_type>
bool msga::io::elf_impl<elf_type>::dump(std::ostream &os){
    rebuild_relocation_table();
    // IF PT_LOAD, offset%page_size==vaddr%page_size

    std::map<offset_t,size_t>addr_map;


    // offset_t ph_start=sizeof(m_ident)+sizeof(m_ehdr_h);
    // auto ph_num=m_programs.size();
    // auto ph_size=ph_num*sizeof(program_t::phdr);//m_ehdr_h.e_phnum
    // auto ph_size=m_ehdr_h.e_phnum*sizeof(program_t::phdr);//
    // auto data_start=ph_start+ph_size;

    // os.seekp(data_start);
    
    for(auto&section:m_sections){
        if(section->shdr.sh_type==Elf_SHT::SHT_NULL){
            continue;
        }
        if(section->shdr.sh_addr){
            if(section->shdr.sh_offset){
                os.seekp(section->shdr.sh_offset);
            }
            auto s_start=floor(static_cast<offset_t>(os.tellp()),MSGA_ELF_PAGE_SIZE)+ section->shdr.sh_addr%MSGA_ELF_PAGE_SIZE;
            if(s_start<os.tellp()){
                s_start+=MSGA_ELF_PAGE_SIZE;
            }
            if(s_start!=os.tellp()){
                os.seekp(s_start);
            }
            section->shdr.sh_offset=s_start;
        }else{
            if(section->shdr.sh_addralign){
                os.seekp(aligned(static_cast<offset_t>(os.tellp()),section->shdr.sh_addralign));
            }
            section->shdr.sh_offset=os.tellp();
        }
        if(section->shdr.sh_type!=Elf_SHT::SHT_NOBITS){
            section->shdr.sh_size=section->data.size();
        }
        if(auto link=section->link.lock()){
            for(auto i=0;i<m_sections.size();++i){
                if(link==m_sections[i]){
                    section->shdr.sh_link=i;
                    break;
                }
            }
        }
        os<<section->data;
    }

    auto sh_start=aligned(static_cast<size_t>(os.tellp()),static_cast<size_t>(arch()));
    


    os.seekp(sh_start);
    for(auto&section:m_sections){
        os<<section->shdr;
    }
    auto p_align=sizeof(elf_type::phdr_t::p_vaddr);
    auto ph_num=m_programs.size()+2;
    auto ph_size=ph_num*sizeof(program_t::phdr);
    auto ph_addr=reserved(0,ph_size,MSGA_ELF_PAGE_SIZE);
    offset_t ph_start=congruence_modulo<offset_t>(os.tellp(),ph_addr,MSGA_ELF_PAGE_SIZE);
    os.seekp(ph_start);
    {
        //https://docs.oracle.com/cd/E19683-01/816-1386/6m7qcoblk/index.html#chapter6-37
        addr_t min_addr=-1;
        for(auto&prgo:m_programs){
            if(prgo.phdr.p_type==Elf_PT::PT_LOAD){
                min_addr=std::min(min_addr,(addr_t)prgo.phdr.p_vaddr);
            }
        }
        if(min_addr==-1){
            return false;
        }
        min_addr=floor(min_addr,p_align);
        ph_addr=ph_start+min_addr;
        typename elf_type::phdr_t phdr;
        phdr.p_type=Elf_PT::PT_PHDR;
        phdr.p_flags=Elf_PF::PF_R;
        phdr.p_paddr=ph_addr;
        phdr.p_vaddr=ph_addr;
        phdr.p_filesz=ph_size;
        phdr.p_memsz=ph_size;
        phdr.p_offset=ph_start;
        phdr.p_align=p_align;
        os<<phdr;
    }
    for(auto&prgo:m_programs){
        size_t ph_start=(-1),ph_end=0;
        for(auto&section_ptr:prgo.sections){
            auto section=section_ptr.lock();
            if(!section){
                continue;
            }
            ph_start=std::min<size_t>(ph_start,section->shdr.sh_offset);
            ph_end=std::max<size_t>(ph_end,section->shdr.sh_offset+section->shdr.sh_size);
        }
        auto p_size=ph_end-ph_start;
        prgo.phdr.p_offset=ph_start;
        prgo.phdr.p_filesz=p_size;
        prgo.phdr.p_memsz=p_size;
        os<<prgo.phdr;
    }
    {
        typename elf_type::phdr_t load_phdr;
        load_phdr.p_type=Elf_PT::PT_LOAD;
        load_phdr.p_flags=Elf_PF::PF_R;
        load_phdr.p_paddr=ph_addr;
        load_phdr.p_vaddr=ph_addr;
        load_phdr.p_filesz=ph_size;
        load_phdr.p_memsz=ph_size;
        load_phdr.p_offset=ph_start;
        load_phdr.p_align=MSGA_ELF_PAGE_SIZE;
        os<<load_phdr;
    }

    auto shstr_index=0;
    if(auto shstr=m_shstrtab_ref.lock()){
        for(auto i=0;i<m_sections.size();++i){
            if(m_sections[i]==shstr){
                shstr_index=i;
                break;
            }
        }
    }



    m_ehdr_h.e_ehsize=sizeof(m_ident)+sizeof(m_ehdr_h);
    m_ehdr_h.e_phoff=ph_start;
    m_ehdr_h.e_phnum=ph_num;
    m_ehdr_h.e_phentsize=sizeof(program_t::phdr);
    m_ehdr_h.e_shoff=sh_start;
    m_ehdr_h.e_shnum=m_sections.size();
    m_ehdr_h.e_shentsize=sizeof(section_t::shdr);
    m_ehdr_h.e_shstrndx=shstr_index;
    os.seekp(std::ios::beg);
    os<<m_ident;
    os<<m_ehdr_h;
    return true;
}

bool msga::io::elf_base::load(std::string const&filename){
    std::ifstream is(filename,std::ios::binary);
    return load(is);
}
bool msga::io::elf_base::dump(std::string const&filename){
    std::ofstream os(filename,std::ios::binary);
    return dump(os);

}
template<class elf_type>
typename msga::io::elf_impl<elf_type>::section_t&msga::io::elf_impl<elf_type>::shstrtab(){
    if(auto shstrtab=m_shstrtab_ref.lock()){
        return *shstrtab;
    }
    auto shstrtab=std::make_shared<section_t>();
    shstrtab->shdr.sh_type=Elf_SHT::SHT_STRTAB;
    shstrtab->shdr.sh_addralign=1;
    m_shstrtab_ref=shstrtab;
    m_sections.emplace_back(shstrtab);
    return *shstrtab;
}
template<class elf_type>
typename msga::io::elf_impl<elf_type>::section_t*msga::io::elf_impl<elf_type>::section(std::string const&name,section_t*start){
    auto&shstrtab=elf_impl::shstrtab();
    for(auto&section:m_sections){
        if(start){
            if(section.get()==start){
                start=nullptr;
            }
            continue;
        }
        if(section->shdr.sh_name+name.size()>shstrtab.data.size()){
            continue;
        } 
        if(memcmp(&shstrtab.data[section->shdr.sh_name],name.data(),name.size())){
            continue;
        }
        return section.get();
    }
    return nullptr;
}
template<class elf_type>
typename msga::io::elf_impl<elf_type>::section_t&msga::io::elf_impl<elf_type>::section(addr_t addr){
    for(auto&section:m_sections){
        auto sh_start=section->shdr.sh_addr;
        auto sh_end=sh_start+section->shdr.sh_size;
        if(addr>=sh_start&&addr<sh_end){
            return *section;
        }
    }
    throw std::runtime_error("address out of range");
}

template<class elf_type>
msga::addr_t msga::io::elf_impl<elf_type>::reserved(addr_t preferred, size_t len,size_t align)
{
    len = aligned(len, align);

    if(!preferred){
        addr_t max_end=0;
        for (auto &section : m_sections)
        {
            auto cur_end=section->shdr.sh_addr+section->shdr.sh_size;
            max_end=std::max<addr_t>(max_end,cur_end);
        }
        return aligned(max_end,align);
    }

    std::map<addr_t, size_t> map;
    for (auto &section : m_sections)
    {
        map.emplace(section->shdr.sh_addr, section->shdr.sh_size);
    }
    preferred = aligned(preferred, align);
    
    addr_t last_end = 0;
    addr_t best_dist = preferred;
    addr_t best_end = last_end;

    for (auto &item : map)
    {
        last_end=aligned(last_end, align);
        addr_t cur_end = item.first + item.second;
        auto space=item.first - last_end;
        if (last_end>item.first||space < len)
        {
            last_end = cur_end;
            continue;
        }
        auto cur_dist = std::abs((int64_t)(preferred - last_end));
        if (cur_dist < best_dist)
        {
            best_end = last_end;
            best_dist = cur_dist;
        }
        last_end = cur_end;
    }
    
    if (best_end == 0)
    {
        best_end = last_end ? last_end : preferred;
    }
    auto result=aligned(best_end, align);
    return result;
}

template<class elf_type>
std::shared_ptr<typename msga::io::elf_impl<elf_type>::section_t> msga::io::elf_impl<elf_type>::make_section(std::string const&name,section_option_t const&option){
    auto&shstrtab=elf_impl::shstrtab();
    auto it=std::search(std::begin(shstrtab.data),std::end(shstrtab.data),std::begin(name),std::end(name));
    size_t name_index=0;
    if(it==std::end(shstrtab.data)){
        name_index=shstrtab.data.size();
        shstrtab.data<<name;
    }else{
        name_index=it-std::begin(shstrtab.data);
    }
    auto size=aligned(option.size,option.sh_align);
    auto section=std::make_shared<section_t>();
    section->shdr.sh_name=name_index;
    section->shdr.sh_flags=option.sh_flag;
    section->shdr.sh_addralign=option.sh_align;
    section->shdr.sh_type=option.sh_type;
    section->shdr.sh_size=size;
    section->shdr.sh_addr=reserved(option.perferred,size,MSGA_ELF_PAGE_SIZE);
    section->data.resize(size);
    program_t program;
    program.phdr.p_align=option.p_align;
    program.phdr.p_type=option.p_type;
    program.phdr.p_vaddr=section->shdr.sh_addr;
    program.phdr.p_paddr=section->shdr.sh_addr;
    program.phdr.p_flags=option.p_flag;
    program.sections.emplace_back(section);
    m_programs.emplace_back(std::move(program));
    m_sections.emplace_back(section);     
    return section;   
}

template<class elf_type>
addr_t msga::io::elf_impl<elf_type>::alloc(size_t size, addr_t preferred){
    {
        section_t*section=elf_impl::section(MSGA_SECTION_NAME);
        while(section){
            auto addr=allocate(section->data.data(),section->data.size(),size,static_cast<size_t>(arch()));
            if(addr){
                auto off=static_cast<uint8_t*>(addr)-section->data.data();
                return (section->shdr.sh_addr+off);
            }
            section=elf_impl::section(MSGA_SECTION_NAME,section);
        }
    }
    section_option_t option;
    option.sh_align=static_cast<size_t>(arch());
    option.sh_flag=static_cast<Elf_SHF>(Elf_SHF::SHF_ALLOC|Elf_SHF::SHF_EXECINSTR);
    option.sh_type=Elf_SHT::SHT_PROGBITS;

    option.p_align=MSGA_ELF_PAGE_SIZE;
    option.p_type=Elf_PT::PT_LOAD;
    option.p_flag=static_cast<Elf_PF>(Elf_PF::PF_X|Elf_PF::PF_R);
    option.perferred=preferred;
    option.size=size+4;
    auto section=make_section(MSGA_SECTION_NAME,option);
    auto addr=allocate(section->data.data(),section->data.size(),size,section->shdr.sh_addralign);
    auto off=static_cast<uint8_t*>(addr)-section->data.data();
    return (section->shdr.sh_addr+off);
}
template<class elf_type>
void msga::io::elf_impl<elf_type>::free(addr_t addr,size_t len){
    deallocate(&get<uint8_t>(addr));
}
//TODO: RELA
template<class elf_type>
void msga::io::elf_impl<elf_type>::rebase(addr_t addr, rel_base_t*opt){
    auto type=0;
    addr=addr+opt->offset();
    relx_t data;
    if(e_rel::e_rel_sys==opt->type()){
        data=dynamic_cast<rel_elf_t*>(opt)->data();
    }else{
        if(arch()==io::mode::x64){
            switch (opt->type())
            {
            case e_rel::e_rela:type=Elf_R_AMD64::R_AMD64_GLOB_DAT; break;
            case e_rel::e_rel16:type=Elf_R_AMD64::R_AMD64_16; break;
            case e_rel::e_rel32:type=Elf_R_AMD64::R_AMD64_32; break;
            case e_rel::e_rel64:type=Elf_R_AMD64::R_AMD64_64; break;
            default:
                throw std::runtime_error("bad e_rel type");
                break;
            }
        }else{
            switch (opt->type())
            {
            case e_rel::e_rela:type=Elf_R_386::R_386_RELATIVE; break;
            case e_rel::e_rel16:type=Elf_R_386::R_386_16; break;
            case e_rel::e_rel32:type=Elf_R_386::R_386_32; break;
            default:
                throw std::runtime_error("bad e_rel type");
            }
        }
        data=relx_t(type,get<typename relx_t::addend_t>(addr));
    }

    if(m_rel_table.empty()){
        section_option_t option;
        option.sh_align=static_cast<size_t>(arch());
        option.sh_flag=static_cast<Elf_SHF>(Elf_SHF::SHF_ALLOC|Elf_SHF::SHF_EXECINSTR);
        option.sh_type=Elf_SHT::SHT_REL;

        option.p_align=MSGA_ELF_PAGE_SIZE;
        option.p_type=Elf_PT::PT_LOAD;
        option.p_flag=static_cast<Elf_PF>(Elf_PF::PF_R);
        option.perferred=0;
        option.size=MSGA_ELF_PAGE_SIZE;
        auto section=make_section(MSGA_REL_SECTION_NAME,option);
        m_rel_table[section.get()][addr]=data;
    }else{
        std::begin(m_rel_table)->second[addr]=data;
    }

}
template<class elf_type>
void msga::io::elf_impl<elf_type>::debase(addr_t addr){
    for(auto&map:m_rel_table){
        auto it=std::begin(map.second);
        while (it!=std::end(map.second)){
            if(it->first==addr){
                map.second.erase(it);
                return;
            }
            it++;
        }
    }
}
template<class elf_type>
void msga::io::elf_impl<elf_type>::search_rebase(rel_list_t &items, addr_t addr, size_t len){
    auto end=addr+len;
    for(auto&map:m_rel_table){
        for(auto&item:map.second){
            if(item.first>=addr&&item.first<end){
                items.emplace_back(std::make_unique<rel_elf_t>(item.first,relx_t(item.second)));
            }
        }
    }
}
template<class elf_type>
void msga::io::elf_impl<elf_type>::range_rebase(rel_list_t const &items, addr_t addr, size_t len){
    auto end=addr+len;
    for(auto&map:m_rel_table){
        auto it=std::begin(map.second);

        while (it!=std::end(map.second))
        {
            if(it->first>=addr&&it->first<end){
                it=map.second.erase(it);
            }else{
                it++;
            }
        }
    }
    for(auto&item:items){
        rebase(addr,item.get());
    }
}


bool msga::io::elf::load(std::istream &is){
    is.seekg(std::ios::beg);
    Elf_Ident ident;
    is>>ident;
    switch(ident.e_class){
        case Elf_Class::EC_ELF32:m_impl=std::make_shared<elf_impl<elf32>>();break;
        case Elf_Class::EC_ELF64:m_impl=std::make_shared<elf_impl<elf64>>();break;
        default:
        return false;
    }
    return m_impl->load(is);
}
