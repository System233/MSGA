// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "msga/arch/x86_64.h"
#include "msga/io/runtime.h"
#include "msga/code.h"
#include "msga/types.h"
#include "msga/manager.h"
#include "test.h"
#include <cstdio>

using namespace msga::io;
using namespace msga;
#define ADD_X 123456
#define ADD_Y 0x789ABC
#define EXCEPT_RESULT (ADD_X+ADD_Y)
#define HOOKED_RESULT (ADD_X*2+ADD_Y+1)



int test_add(int x,int y){
    printf("[test_add] %d,%d->%d\n",x,y,x+y);
    return x+y;
}

using PFunc=int(int,int);
PFunc*pfunc_add;
int hooked_add(int x,int y){
    printf("[hooked_add:%p:%p] %d,%d\n",hooked_add,pfunc_add,x,y);
    TEST(pfunc_add!=nullptr);
    auto ret=pfunc_add(x*2,y+1);
    TEST(ret==x*2+y+1);
    return ret;
}

int main(int argc, char const *argv[])
{
    msga::arch::x86_64 arch;
    msga::io::runtime io;
    io::base&iobase=io;
    msga::manager man(arch,io);
    
    auto before=test_add(ADD_X,ADD_Y);
    TEST(before==ADD_X+ADD_Y);
    TEST_EQ(before,EXCEPT_RESULT);

    auto hook_err=man.dohook((addr_t)&test_add,(addr_t)&hooked_add,(addr_t)&pfunc_add);
    TEST(hook_err);

    
    hook_err=man.dohook((addr_t)&test_add,(addr_t)&hooked_add,(addr_t)&pfunc_add);
    TEST_F(hook_err);

    auto hooked=test_add(ADD_X,ADD_Y);
    TEST_EQ(hooked,HOOKED_RESULT);

    hook_err=man.unhook(&test_add,&hooked_add);
    TEST(hook_err);
    
    auto end=test_add(ADD_X,ADD_Y);
    TEST_EQ(end,EXCEPT_RESULT);
    
    return 0;
}

