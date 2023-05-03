// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include <stdio.h>
#include <cstdint>
#include <fstream>
#include "test.h"



extern "C" int test_add(int x,int y){
    printf("[test_add] %d,%d->%d\n",x,y,x+y);
    return x+y;
}

using PFunc=int(int,int);
extern "C" PFunc*pfunc_add;
PFunc*pfunc_add;
extern "C" int hooked_add(int x,int y){
    printf("[hooked_add:%p:%p] %d,%d\n",hooked_add,pfunc_add,x,y);
    TEST(pfunc_add!=nullptr);
    auto ret=pfunc_add(x*2,y+1);
    TEST(ret==x*2+y+1);
    return ret;
}

int main(int argc, char const *argv[])
{
    if(argc!=4){
        SHOW_ARGS(argc,argv);
        fprintf(stderr,"%s <number:expect> <number:x> <number:y>\n",argv[0]);
        exit(-1);
    }
    int expect=atoi(argv[1]);
    int x=atoi(argv[2]);
    int y=atoi(argv[3]);
    int ret=test_add(x,y);
    TEST_EQ(expect,ret);
    return 0;
}
