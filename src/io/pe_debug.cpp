// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include "msga/io/pe.h"
#include <iostream>
void msga::io::pe::debug()const{
    std::cout<<"m_rel_table.size():"<<m_rel_table.size()<<std::endl;
    for(auto&item:m_rel_table){
        printf("[reloc] %zx->%zu\n",item.first,item.second.size());
    }
    std::cout<<"m_sections.size():"<<m_sections.size()<<std::endl;
    for(auto&item:m_sections){
        printf("[sections] %zx->%zu\n",item.addr(),item.size());
    }
    std::cout<<"m_symbol.data.size():"<<m_symbol.data.size()<<std::endl;
    printf("[symbol] %zu,%zu\n",m_symbol.table.size(),m_symbol.data.size());
}