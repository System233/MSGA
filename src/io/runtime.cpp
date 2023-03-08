/**
 * Copyright (c) 2023 System233
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "msga/io/runtime.h"
#include <algorithm>


msga::io::mode msga::io::runtime::arch()const{
#if defined(__x86_64__)||defined(_WIN64)
    return msga::io::mode::x64;
#elif defined(__i386__)||defined(_WIN32)
    return msga::io::mode::x32;
#else
    return msga::io::mode::unknown;
#endif
};
msga::io::mode msga::io::runtime::mode()const{
    return arch();
};

#if defined(__unix__)
#include <sys/mman.h>
#include <unistd.h>
#include <fstream>
#include <string>
static const int page_size=getpagesize();
int range_protect(msga::addr_t addr,size_t len){
    std::ifstream is("/proc/self/maps");
    size_t start=0,end=0;
    char r,w,x;
    while((is>>std::hex>>start).ignore(1,'-')>>end>>r>>w>>x){
        if(addr>=start&&addr+len<=end){
            int prot=PROT_NONE;
            if(r!='-'){
                prot|=PROT_READ;
            }
            if(w!='-'){
                prot|=PROT_WRITE;
            }
            if(x!='-'){
                prot|=PROT_EXEC;
            }
            return prot;
        }
        is.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    throw std::runtime_error("range_protect bad protect");
}
void range_mprotect(msga::addr_t addr,size_t len,int prot){
    auto start=msga::floor(addr,page_size);
    len=msga::aligned(len+addr-start,page_size);
    auto result=mprotect(reinterpret_cast<void*>(start),len,prot);
    if(result==-1){
        throw std::runtime_error("mprotect fail");
    }
}
void msga::io::runtime::read(void*data,addr_t addr,size_t len){
    int raw=range_protect(addr,len);
    if(!(raw&PROT_READ)){
        range_mprotect(addr,len,raw|PROT_READ);
    }
    auto result=reinterpret_cast<char*>(data);
    auto it=reinterpret_cast<char*>(addr);
    std::copy(it,it+len,result);
    if(!(raw&PROT_READ)){
        range_mprotect(addr,len,raw);
    }
}
void msga::io::runtime::write(void const*data, addr_t addr,size_t len){
    int raw=range_protect(addr,len);
    if(!(raw&PROT_WRITE)){
        range_mprotect(addr,len,raw|PROT_WRITE);
    }
    auto result=reinterpret_cast<char*>(addr);
    auto it=reinterpret_cast<char const*>(data);
    std::copy(it,it+len,result);
    if(!(raw&PROT_WRITE)){
        range_mprotect(addr,len,raw);
    }
}
msga::addr_t msga::io::runtime::alloc(size_t len,addr_t preferred){
    auto mem=mmap(reinterpret_cast<void*>(preferred),len,PROT_EXEC|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    return reinterpret_cast<addr_t>(mem);
}
void msga::io::runtime::free(addr_t addr,size_t len){
    munmap(reinterpret_cast<void*>(addr),len);
}
#elif defined(_WIN32)
#include <memoryapi.h>
#include <sysinfoapi.h>

int getpagesize(){
    SYSTEM_INFO info;
    GetNativeSystemInfo(&info);
    return info.dwPageSize;
}
static const int page_size=getpagesize();


void msga::io::runtime::read(void*data,addr_t addr,size_t len){

    auto result=reinterpret_cast<char*>(data);
    auto it=reinterpret_cast<char*>(addr);

    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(it, &mbi,len);
    DWORD oldProtect=0;
    if(mbi.Protect==PAGE_NOACCESS){
        VirtualProtect(it,len,PAGE_READONLY,&oldProtect);
    }else if(mbi.Protect==PAGE_EXECUTE){
        VirtualProtect(it,len,PAGE_EXECUTE_READ,&oldProtect);
    }
    std::copy(it,it+len,result);
    if(oldProtect){
        VirtualProtect(it,len,mbi.Protect,&oldProtect);
    }
}
#include <cstdio>
void msga::io::runtime::write(void const*data, addr_t addr,size_t len){
    
    auto result=reinterpret_cast<char*>(addr);
    auto it=reinterpret_cast<char const*>(data);
    MEMORY_BASIC_INFORMATION mbi{0};
    VirtualQuery(result, &mbi,sizeof(mbi));
    DWORD oldProtect=0;
    if(mbi.Protect<PAGE_READWRITE){
        VirtualProtect(result,len,PAGE_READWRITE,&oldProtect);
    }else if(mbi.Protect>=PAGE_EXECUTE&&mbi.Protect<PAGE_EXECUTE_READWRITE){
        VirtualProtect(result,len,PAGE_EXECUTE_READWRITE,&oldProtect);
    }else if(mbi.Protect>PAGE_EXECUTE_WRITECOPY){
        throw std::runtime_error("unknown memory protect");
    }
    std::copy(it,it+len,result);
    if(oldProtect){
        VirtualProtect(result,len,mbi.Protect,&oldProtect);
    }
};
msga::addr_t msga::io::runtime::alloc(size_t len,addr_t preferred){
    //TODO : RANGE 256MB
    addr_t base=aligned(preferred,page_size);
    for(auto i=0;i<0x10000;++i){
        auto addr=(addr_t)VirtualAlloc((LPVOID)(base+i*0x1000),len,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READ);
        if(addr){
            return addr;
        }
    }
    throw std::runtime_error("io alloc fail");
};
void msga::io::runtime::free(addr_t addr,size_t len){
    VirtualFree(reinterpret_cast<void*>(addr),0,MEM_RELEASE);
};

#endif
