// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <vector>

#include "msga/arch/x86_64.h"
#include "msga/io.h"
#include "msga/code.h"
#include "msga/types.h"
#include "msga/io/elf.h"
#include "msga/manager.h"
using namespace msga::io;
using namespace msga;
void test(int val){
    printf("test func! %d\n",val);
};
using PFunc=void(int);
PFunc*pfunc;
void hooked(int val){
    printf("hooked func! %d\n",val);
    printf("origin func %p\n",pfunc);
    pfunc(val+1);
}
int main(int argc, char const *argv[])
{
    msga::arch::x86_64 arch;
    msga::io::elf io;
    printf("[test elf!]\n");
    if(!io.load("msga_drone")){
        printf("load fail\n");
    }
    msga::manager man(arch,io);
#define PFUNC 0x404040
#define TARGET 0x401160
#define TO 0x4012a0
    man.dohook(TARGET,TO,PFUNC);
    // auto addr=io.alloc(32);
    // auto ptr1=(uint64_t*)io.get(ADDR);
    // ptr1[0]=TARGET;
    // // ptr1[1]=0x22222222;

    // rel_base_t rel(0,e_rel::e_rela);


    // io.rebase(ADDR,&rel);
    // io.rebase(addr+8,&rel);
    // printf("ELF alloc:%016zx\n",(size_t)addr);
    if(!io.dump("msga_drone2")){
        printf("dump fail\n");
    }
    
    printf("[test elf end!]\n");
    // msga::manager man(arch,io);
    
    return 0;
}

