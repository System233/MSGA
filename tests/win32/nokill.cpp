// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include <windows.h>
#include <handleapi.h>
#include <libloaderapi.h>
#include <process.h>
#include <winuser.h>
#include <processthreadsapi.h>
#include "msga/arch/x86_64.h"
#include "msga/io/runtime.h"
#include "msga/manager.h"
#include <stdio.h>
using PTerminateProcess=WINBOOL WINAPI(HANDLE hProcess, UINT uExitCode);
PTerminateProcess*pTerminateProcess;
WINBOOL WINAPI HookedTerminateProcess(HANDLE hProcess, UINT uExitCode){
    auto ret=MessageBoxA(NULL,"Please don't kill me!","Kill Process",MB_OKCANCEL|MB_ICONWARNING);
    if(ret==IDCANCEL){
        SetLastError(5);
        return FALSE;
    }
    return pTerminateProcess(hProcess,uExitCode);
}

void SetupHook(){
    msga::arch::x86_64 arch;
    msga::io::runtime io;
    msga::manager man(arch,io);
    man.dohook(&TerminateProcess,&HookedTerminateProcess,&pTerminateProcess);
    fprintf(stderr,"NoKill Loaded\n");
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved ) 
{
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
            SetupHook();
            break;
    }
    return TRUE; 
}
