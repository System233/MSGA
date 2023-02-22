// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#ifndef _MSGA_IO_H
#define _MSGA_IO_H
#include "msga_type.h"
typedef struct msga_context_t msga_context_t;
typedef MSGA_ERR (*msga_mprotect_t)(msga_addr_t addr, int len, MSGA_MP prot, void *user);
typedef msga_addr_t (*msga_mmap_t)(msga_addr_t addr, int len, MSGA_MP prot, void *user);
typedef MSGA_ERR (*msga_munmap_t)(msga_addr_t addr, int len, void *user);
typedef int (*msga_read_t)(msga_addr_t addr, void *data, int len, void *user);
typedef int (*msga_write_t)(msga_addr_t addr, void const *data, int len, void *user);
typedef void (*msga_done_t)(void *user);
// typedef void (*msga_open_t)(void *user);

typedef struct
{
    msga_mprotect_t mprotect;
    msga_mmap_t mmap;
    msga_munmap_t munmap;
    msga_read_t read;
    msga_write_t write;
    msga_done_t done;
    MSGA_ARCH arch;
    void *user;
} msga_context_t;

MSGA_EXTERN MSGA_ERR msga_mprotect(msga_context_t *ctx, msga_addr_t addr, int len, MSGA_MP prot);
MSGA_EXTERN msga_addr_t msga_mmap(msga_context_t *ctx, msga_addr_t addr, int len, MSGA_MP prot);
MSGA_EXTERN MSGA_ERR msga_munmap(msga_context_t *ctx, msga_addr_t addr, int len);
MSGA_EXTERN int msga_read(msga_context_t *ctx, msga_addr_t addr, void *data, int len);
MSGA_EXTERN int msga_write(msga_context_t *ctx, msga_addr_t addr, void const *data, int len);

// MSGA_EXTERN MSGA_ERR msga_init(msga_context_t *ctx, void *user);
MSGA_EXTERN MSGA_ERR msga_init_rt(msga_context_t *ctx, int pid);
MSGA_EXTERN MSGA_ERR msga_init_elf(msga_context_t *ctx, char const*file);
MSGA_EXTERN MSGA_ERR msga_init_pe(msga_context_t *ctx, char const*file);
MSGA_EXTERN MSGA_ERR msga_done(msga_context_t *ctx);

#endif //_MSGA_IO_H