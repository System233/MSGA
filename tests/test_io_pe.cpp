// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "msga/arch/x86_64.h"
#include "msga/io.h"
#include "msga/code.h"
#include "msga/types.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include "msga/io/pe.h"
#include <memoryapi.h>
#include "msga/manager.h"
#include <fstream>
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
    msga::io::pe io;
    if(!io.load("msga_drone.exe")){
        printf("load fail\n");
        return -1;
    }
    msga::manager man(arch,io);
    printf("[test!pe]\n");
    
    man.dohook(io.RVA2VA(0x15b0),io.RVA2VA(0x16a0),io.RVA2VA(0xd044));

    io.debug();
    
    if(!io.dump("msga_drone2.exe")){
        printf("dump fail\n");
        return -1;
    }
    return 0;
}

