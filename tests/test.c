// Copyright (c) 2022 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include <stdio.h>
#include <assert.h>
#include "msga.h"
#include "test.h"
int add(int x,int y){
    return x+y;
}
int mul(int x,int y){
    return x*y;
}

int (*$hook_func)(int x,int y);
int hook_func(int x,int y){
    return $hook_func(x+1,y+1);
}


int (*$hook2_func)(int x,int y);
int hook2_func(int x,int y){
    return $hook2_func(x+2,x+2);
}

int (*$hook3_func)(int x,int y);
int hook3_func(int x,int y){
    return mul(x,y);
}

void testHook(){
    msga_context_t ctx;
    msga_hook_t hook;
    assert(msga_init(&ctx,0)==MSGA_ERR_OK);
    msga_hook_init(&hook,&ctx,(msga_addr_t)add,(msga_addr_t)hook_func,(msga_addr_t)&$hook_func);
    assert(msga_hook_setup(&hook)==MSGA_ERR_OK);

    assert(add(10,20)==30);

    assert(msga_dohook(&hook)==MSGA_ERR_OK);
    assert(add(10,20)==32);

    assert(msga_unhook(&hook)==MSGA_ERR_OK);
    assert(add(10,20)==30);

    assert(msga_hook_free(&hook,0)==MSGA_ERR_OK);
}
void testNestHook(){
    msga_context_t ctx;
    msga_hook_t hook,hook2;
    assert(msga_init(&ctx,0)==MSGA_ERR_OK);
    msga_hook_init(&hook,&ctx,(msga_addr_t)add,(msga_addr_t)hook_func,(msga_addr_t)&$hook_func);
    msga_hook_init(&hook2,&ctx,(msga_addr_t)add,(msga_addr_t)hook2_func,(msga_addr_t)&$hook2_func);
    assert(msga_hook_setup(&hook)==MSGA_ERR_OK);

    assert(add(10,20)==30);

    assert(msga_dohook(&hook)==MSGA_ERR_OK);
    assert(add(10,20)==32);
    
    assert(msga_hook_setup(&hook2)==MSGA_ERR_OK);
    assert(msga_dohook(&hook2)==MSGA_ERR_OK);
    assert(add(10,20)==36);
    
    assert(msga_unhook(&hook)!=MSGA_ERR_OK);

    assert(msga_unhook(&hook2)==MSGA_ERR_OK);
    assert(add(10,20)==32);
    assert(msga_unhook(&hook)==MSGA_ERR_OK);
    assert(add(10,20)==30);
    
    assert(msga_hook_free(&hook,0)==MSGA_ERR_OK);
}

// #include <stdio.h>
// #include <signal.h>
// #include <unistd.h>
// #include <unwind.h>
// #include <dlfcn.h>

// typedef struct{
//     int index;
//     int size;
//     void**frames;
// }Context;
// static _Unwind_Reason_Code callback(struct _Unwind_Context* context, void* user)
// {
//     void* pc = (void*)_Unwind_GetIP(context);
//     Context*ctx=user;
//     if(ctx->index<ctx->size){
//         ctx->frames[ctx->index++]=pc;
//         return _URC_NO_REASON;
//     }
//     return _URC_END_OF_STACK;
// }
// int backtrace(void**buffer,int size){
//     Context ctx={0,size,buffer};
//     _Unwind_Backtrace(callback,&ctx);
//     return ctx.index;
// }
// void dump(void** frames, int size)
// {
//     for (int i = 0; i < size; ++i) {
//         const void* addr = frames[i];
//         Dl_info info={0};
//         if (dladdr(addr, &info) && info.dli_sname) {
//             fprintf(stderr,"  #%d: %p\t%s+%d\n",i,addr,info.dli_sname,addr-info.dli_saddr);
//         }
//         fprintf(stderr,"  #%d: %p\tunknown\n",i,addr);
//     }
//     fflush(stderr);
//     // char buffer[32];
//     // FILE*fp=fopen("/proc/self/maps","rb");
//     // int i=0;
//     // while((i=fread(buffer,1,sizeof(buffer)-1,fp))>0){
//     //     buffer[i]=0;
//     //     fprintf(stderr,"%s",buffer);
//     // }
//     // fflush(fp);
//     // fclose(fp);
    
// }

// void handler(int sig) {
//   void *frames[10];
//   size_t size;
  
//   size = backtrace(frames, 10);
  
//   fprintf(stderr, "Error: signal %d:\n", sig);
//   dump(frames,size);
// //   backtrace_symbols_fd(frames, size, STDERR_FILENO);
//   exit(1);
// }

#define DBG(X) printf(#X"=%d\n",X);
int main(int argc, char const *argv[])
{
    // signal(SIGSEGV, handler);
    // testHook();
    // testNestHook();
    // MSGA_LOG("log:");
    printf("add:%p\n",&add);
    msga_context_t ctx;
    msga_hook_t hook;
    DBG(msga_init(&ctx,0));
    msga_hook_init(&hook,&ctx,(msga_addr_t)add,(msga_addr_t)hook_func,(msga_addr_t)&$hook_func);
    assert(msga_hook_setup(&hook)==MSGA_ERR_OK);
    msga_hook_debug(&hook);
    printf("10+20=%d\n",add(10,20));
    DBG(msga_dohook(&hook));
    printf("10+20=%d\n",add(10,20));
    DBG(msga_unhook(&hook));
    DBG(msga_hook_free(&hook,0));
    return 0;
}
