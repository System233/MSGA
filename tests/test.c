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

    assert(msga_hook_init(&ctx,&hook,(msga_addr_t)add,(msga_addr_t)hook_func,(msga_addr_t)&$hook_func)==MSGA_ERR_OK);

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
    assert(msga_hook_init(&ctx,&hook,(msga_addr_t)add,(msga_addr_t)hook_func,(msga_addr_t)&$hook_func)==MSGA_ERR_OK);

    assert(add(10,20)==30);

    assert(msga_dohook(&hook)==MSGA_ERR_OK);
    assert(add(10,20)==32);
    
    assert(msga_hook_init(&ctx,&hook2,(msga_addr_t)add,(msga_addr_t)hook2_func,(msga_addr_t)&$hook2_func)==MSGA_ERR_OK);
    assert(msga_dohook(&hook2)==MSGA_ERR_OK);
    assert(add(10,20)==36);
    
    assert(msga_unhook(&hook)!=MSGA_ERR_OK);

    assert(msga_unhook(&hook2)==MSGA_ERR_OK);
    assert(add(10,20)==32);
    assert(msga_unhook(&hook)==MSGA_ERR_OK);
    assert(add(10,20)==30);
    
    assert(msga_hook_free(&hook,0)==MSGA_ERR_OK);
}
int main(int argc, char const *argv[])
{
    // testHook();
    // testNestHook();
    // MSGA_LOG("log:");
    printf("add:%p\n",&add);
    msga_context_t ctx;
    msga_hook_t hook;
    msga_init(&ctx,0);
    msga_hook_init(&ctx,&hook,(msga_addr_t)add,(msga_addr_t)hook_func,(msga_addr_t)&$hook_func);
    msga_hook_debug(&hook);
    printf("10+20=%d\n",add(10,20));
    msga_dohook(&hook);
    printf("10+20=%d\n",add(10,20));
;
    
    msga_unhook(&hook);
    msga_hook_free(&hook,0);
    return 0;
}
