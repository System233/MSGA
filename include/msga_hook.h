// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _MSGA_HOOK_H
#define _MSGA_HOOK_H
#include "msga_type.h"
#include "msga_buffer.h"


typedef struct
{
    msga_context_t *context;
    msga_addr_t to_addr;
    msga_addr_t jmp_addr;
    msga_addr_t from_addr;
    msga_buffer_t backup;
    msga_buffer_t newbuf;
    msga_buffer_t jmpbuf;
} msga_hook_t;


MSGA_EXTERN MSGA_ERR msga_hook_free(msga_hook_t *hook);
MSGA_EXTERN MSGA_ERR msga_hook_init(msga_hook_t *hook, msga_context_t *ctx, msga_addr_t to_addr, msga_addr_t from_addr);
MSGA_EXTERN MSGA_ERR msga_dohook(msga_hook_t *hook);
MSGA_EXTERN MSGA_ERR msga_unhook(msga_hook_t *hook);



MSGA_EXTERN MSGA_ERR msga_hook_setup(msga_hook_t *hook);

#endif //_MSGA_HOOK_H