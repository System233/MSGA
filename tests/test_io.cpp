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
#include <memoryapi.h>
#include "msga/manager.h"
using namespace msga::io;
using namespace msga;

class win:public msga::io::base{
public:
        virtual msga::io::mode arch()const override{
#if _WIN64
        return msga::io::mode::x64;
#else
        return msga::io::mode::x32;
#endif
        };
        virtual msga::io::mode mode()const override{
            return msga::io::mode::x32;
        };
        virtual void read(void*data,addr_t addr,size_t len)override{
            memcpy(data,(void*)addr,len);
        }
        void write(void const*data, addr_t addr,size_t len)override{
            DWORD oldProtect;
            VirtualProtect((void*)addr,len,PAGE_EXECUTE_READWRITE,&oldProtect);
            memcpy((void*)addr,data,len);
            VirtualProtect((void*)addr,len,oldProtect,&oldProtect);

        }
        virtual addr_t alloc(size_t size,addr_t perfer=0)override{
            // malloc(size);
            addr_t base=aligned(perfer,0x1000);
            for(auto i=0;i<2000;++i){
                auto addr=(addr_t)VirtualAlloc((LPVOID)(base+i*0x1000),size,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
                if(addr){
                    return addr;
                }
            }
            return 0;
        };
        virtual void free(addr_t addr)override{
            VirtualFree((LPVOID)addr,NULL,MEM_RELEASE);
            // ::free((void*)addr);
        };
        
};
void testFunc3();
void testFunc2(){
    printf("test2!%p\n",testFunc2);
    float pi=1;
    float n=1;
    int j;
    for(j=1;j<=1000;j++,n++){
        if(j%2==0){
            pi*=(n/(n+1));
        }else{
            pi*=((n+1)/n);
        }
    }
    pi=2*pi;
    printf("PI=%.6f\n",pi);
}
void testFunc3();
int test(int x){
    printf("test! %p\n",&test);
    int a=10;
    int b=20;
    printf("a=%d,b=%d,a+b=%d\n",a,b,a+b);
    printf("a=%d,b=%d,a+b+x=%d\n",a,b,a+b+x);
    float f=100*x;
    printf("x*100=%f\n",f);
    for(int i=0;i<10;++i){
        if(i%2){
            printf("%02x ",i);
        }else if(i%3){
            printf("%02x ",i);
        }
    }
    printf("\n");
    int i=10;
    while(i--){
        
        printf("%d ",i);
    }
    printf("\n");
    testFunc2();
    testFunc3();
    printf("ok\n");
    return 0;
}

void testFunc3(){
    printf("test3! %p\n",testFunc3);
    float s=1;
    float pi=0;
    float i=1.0;
    float n=1.0;
    while(std::abs(i)>=1e-6){
        pi+=i;
        n=n+2;
        s=-s;
        i=s/n;
    }
    pi=4*pi;
    printf("PI=%.6f\n",pi);
}
// #pragma code_seg(push, r1, ".text")
__attribute__ ((section (".text")))
char data[4000];
// #pragma code_seg(pop, r1)
void print(void*data){
    printf("%p",data);
    for(int i=0;i<20;++i){
        printf(" %02x",*((uint8_t*)data+i));
    }
    printf("\n");
}
using PFunc=int(int);
PFunc*pfunc;
int mytest(int val){
    printf("mytest %d!\n",val);
    printf("pfunc %p!\n",pfunc);
    // getchar();
    return pfunc(val);
}
int main(int argc, char const *argv[])
{
    msga::arch::x86_64 arch;
    win io;
    io::base&iobase=io;
    msga::manager man(arch,io);
    man.dohook((addr_t)&test,(addr_t)&mytest,(addr_t)&pfunc);
    printf("test %p!\n",&test);
    printf("mytest %p!\n",&mytest);
    printf("pfunc %p!\n",pfunc);
    getchar();
    test(123);
    // std::vector<msga::code>generated,backup;
    // arch.generate(io,(addr_t)&test,(addr_t)&mytest,generated,backup,(addr_t)&pfunc,2000);
    // // generated[0].address=(addr_t)&data;
    // msga::code &co=generated[0];
    // // printf("generated[%zu]\n",generated.size());
    // // printf("co[%zu]\n",co.size());
    // for(auto&co:generated){
    //     if(!co.overwrite()){
    //         // addr_t ptr=io.alloc(co.size());
    //         co.setbase((addr_t)&data);
    //     }
    //     iobase.write(co);
    // }
    // print((void*)&test);
    // print(generated[0].get());
    // print(backup[0].get());
    // print(&data);
    // print(&pfunc);
    // // printf("%p\n%p\n%p",&test,generated[0].get(),backup[0].get());
    // // PFunc*func= (PFunc*)&data;
    // getchar();
    // test(100);
    // // func(100);
    // pfunc(100);
    return 0;
}

