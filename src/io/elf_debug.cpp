 
#include "msga/io/elf.h"
#include <iostream>


template<class elf_type>
void msga::io::elf_impl<elf_type>::debug(){
    std::cout<<"[DEBUG]"<<std::endl;
    for(auto&prog:m_programs){
        std::cout<<"[P] "<<std::hex<<prog.phdr.p_offset<<"->"<<prog.phdr.p_offset+prog.phdr.p_filesz<<" ["<<prog.phdr.p_filesz<<"]"<<std::endl;
        for(auto&section_ptr:prog.sections){
            if(auto section=section_ptr.lock()){
                std::cout<<"  [S] "<<std::hex<<section->shdr.sh_offset<<"->"<<section->shdr.sh_offset+section->shdr.sh_size<<" ["<<section->shdr.sh_size<<"]"<<std::endl;
            }
        }
    }
}
