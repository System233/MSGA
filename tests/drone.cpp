// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include <stdio.h>
#include <cstdint>
#include <windows.h>
void test(int val){
    printf("test func!%p %d\n",&test,val);
    
    int a=10;
    int b=20;
    printf("a=%d,b=%d,a+b=%d\n",a,b,a+b);
    printf("a=%d,b=%d,a+b+x=%d\n",a,b,a+b+val);
    float f=100*val;
    printf("x*100=%f\n",f);
    for(int i=0;i<10;++i){
        if(i%2){
            printf("%02x ",i);
        }else if(i%3){
            printf("%02x ",i);
        }
    }
    printf("\n");
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
    uint8_t*base=(uint8_t*)GetModuleHandle(NULL);
    printf("test:%p, RVA:%zx\n",test,((uint8_t*)&test)-base);
    printf("hooked:%p, RVA:%zx\n",hooked,((uint8_t*)&hooked)-base);
    printf("pfunc:%p, RVA:%zx\n",&pfunc,((uint8_t*)&pfunc)-base);
    test(123);
    return 0;
}
