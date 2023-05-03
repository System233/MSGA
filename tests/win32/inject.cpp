// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include <windows.h>
#include <stdio.h>
int main(int argc, char const *argv[])
{
    if(argc!=3){
        fprintf(stderr,"%s <pid:number> <dll:full path>\n",argv[0]);
        exit(-1);
    }
    DWORD pid=atoi(argv[1]);
    auto dll=argv[2];
    auto len=strlen(dll);

    fprintf(stderr,"inject pid:%lu, dll:%s\n",pid,dll);
    auto hProc=OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
    if(!hProc||hProc==INVALID_HANDLE_VALUE){
        fprintf(stderr,"OpenProcess FAIL\n");
        exit(-1);
    }
    
    fprintf(stderr,"hProc:%p\n",hProc);
    auto addr=VirtualAllocEx(hProc,NULL,len,MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    fprintf(stderr,"VirtualAllocEx:%p\n",addr);
    SIZE_T writen=0;
    WriteProcessMemory(hProc,addr,dll,len,&writen);
    fprintf(stderr,"WriteProcessMemory:%zu\n",writen);
    auto hThread=CreateRemoteThread(hProc,NULL,NULL,(LPTHREAD_START_ROUTINE)&LoadLibraryA,addr,NULL,NULL);
    if(hThread==INVALID_HANDLE_VALUE){
        fprintf(stderr,"CreateRemoteThread FAIL\n");
        CloseHandle(hProc);
        exit(-1);
    }
    fprintf(stderr,"hThread:%p\n",hThread);

    WaitForSingleObject(hThread,-1);
    DWORD exitCode=0;
    GetExitCodeThread(hThread,&exitCode);
    CloseHandle(hThread);
    VirtualFreeEx(hProc,addr,0,MEM_RELEASE);
    CloseHandle(hProc);
    fprintf(stderr,"inject done,result:%s\n",exitCode?"SUCCESS":"FAIL");
    return 0;
}
