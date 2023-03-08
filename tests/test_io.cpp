// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "msga/arch/x86_64.h"
#include "msga/io/runtime.h"
#include "msga/code.h"
#include "msga/types.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include "msga/manager.h"
using namespace msga::io;
using namespace msga;
#define INIT_ARG 100
#define ARG_ADD 123


int test(int x){
    printf("test! %p\n",&test);
    printf("test arg=%d\n",x);
    printf("ok\n");
    return 0;
}

using PFunc=int(int);
PFunc*pfunc;
int hooked(int val){
    printf("[hooked] %d!\n",val);
    printf("[hooked] pfunc %p!\n",pfunc);
    printf("[hooked] ARG= %d!\n",val);
    return pfunc(ARG_ADD);
}
int main(int argc, char const *argv[])
{
    msga::arch::x86_64 arch;
    msga::io::runtime io;
    io::base&iobase=io;
    msga::manager man(arch,io);
    printf("[HOOK BEFORE]\n");
    test(INIT_ARG);
    printf("[HOOK AFTER]\n");
    man.dohook((addr_t)&test,(addr_t)&hooked,(addr_t)&pfunc);
    printf("test %p!\n",&test);
    printf("mytest %p!\n",&hooked);
    printf("pfunc %p!\n",pfunc);
    test(INIT_ARG);
    return 0;
}

