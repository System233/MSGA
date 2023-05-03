// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>
// #include <handleapi.h>
// #include <libloaderapi.h>
// #include <process.h>
// #include <winuser.h>
// #include <processthreadsapi.h>
#include <iostream>
#include "msga/arch/x86_64.h"
#include "msga/io/runtime.h"
#include "msga/manager.h"

LPVOID GetThreadStartAddress(){
    LPVOID lpStartAddress=nullptr;
    ULONG returnLength=sizeof(lpStartAddress);
    auto status=NtQueryInformationThread(GetCurrentThread(),ThreadQuerySetWin32StartAddress,&lpStartAddress,sizeof(lpStartAddress),&returnLength);
    if(NT_ERROR(status)){
        std::cerr<<"NtQueryInformationThread FAIL:"<<status<<std::endl;
    }
    return lpStartAddress;
}

extern "C" NTSTATUS NTAPI NtOpenThread( PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId );
using PLdrLoadDll=NTSTATUS NTAPI(
  IN PWCHAR PathToFile OPTIONAL,
  IN INT_PTR Flags OPTIONAL,
  IN PUNICODE_STRING ModuleFileName,
  OUT PHANDLE ModuleHandle);
PLdrLoadDll*LdrLoadDll;
PLdrLoadDll*pLdrLoadDll=nullptr;

static LPVOID blocklist[]={
    reinterpret_cast<LPVOID>(&LoadLibraryA),
    reinterpret_cast<LPVOID>(&LoadLibraryW),
    reinterpret_cast<LPVOID>(&LoadLibraryExA),
    reinterpret_cast<LPVOID>(&LoadLibraryExW),
    reinterpret_cast<LPVOID>(&LdrLoadDll)
};
NTSTATUS NTAPI HookedLdrLoadDll(
  PWCHAR PathToFile,
  INT_PTR Flags,
  PUNICODE_STRING ModuleFileName,
  PHANDLE ModuleHandle){
    LPVOID lpStartAddress=GetThreadStartAddress();
    if(lpStartAddress){
        for(auto item:blocklist){
            if(item==lpStartAddress){
                std::wcerr<<L"Illegal injection blocked:"<<ModuleFileName->Buffer<<std::endl;
                SetLastError(5);
                return STATUS_ACCESS_DENIED;
            }
        }
    }
    return pLdrLoadDll(PathToFile,Flags,ModuleFileName,ModuleHandle);
}
int main(){
    msga::arch::x86_64 arch;
    msga::io::runtime io;
    msga::manager man(arch,io);
    bool choice=true;
    std::cout<<"Injection Blocker PID:"<<GetProcessId(GetCurrentProcess())<<std::endl;
    auto ntdll=GetModuleHandleA("NTDLL.DLL");
    LdrLoadDll=(PLdrLoadDll*)GetProcAddress(ntdll,"LdrLoadDll");
    CloseHandle(ntdll);

    std::cout<<"LdrLoadDll:"<<(void*)LdrLoadDll<<std::endl;
    do{
        if(choice){
            if(!pLdrLoadDll&&!man.dohook(LdrLoadDll,&HookedLdrLoadDll,&pLdrLoadDll)){
                std::cerr<<"HOOK FAIL!"<<std::endl;
            }
            std::cout<<"pLdrLoadDll:"<<(void*)pLdrLoadDll<<std::endl;
        }else if(pLdrLoadDll){
            if(!man.unhook(LdrLoadDll,&HookedLdrLoadDll)){
                std::cerr<<"UNHOOK FAIL!"<<std::endl;
            }else{
                pLdrLoadDll=0;
            }
        }else{
            std::cerr<<"Not Hook"<<std::endl;
        }
        std::cout<<"Status:"<<(pLdrLoadDll?"Blocked":"Not Blocked")<<">";
    }while (std::cin>>choice);
    
}
