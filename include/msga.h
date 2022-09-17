// Copyright (c) 2022 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _MSGA_H
#define _MSGA_H

#ifdef __cplusplus
#define _extern extern "C"
#else
#define _extern
#endif

#define MSGA_JMPBACK_SIZE 32
#define MSGA_BUFFER_SIZE 64

typedef enum
{
    MSGA_MP_NONE = 0,
    MSGA_MP_READ = 1,
    MSGA_MP_WRITE = (1 << 1),
    MSGA_MP_EXEC = (1 << 2),
    MSGA_MP_READWRITE_EXEC = (MSGA_MP_READ | MSGA_MP_WRITE | MSGA_MP_EXEC),
    MSGA_MP_READWRITE = (MSGA_MP_READ | MSGA_MP_WRITE),
    MSGA_MP_READ_EXEC = (MSGA_MP_READ | MSGA_MP_EXEC),
    MSGA_MP_WRITE_EXEC = (MSGA_MP_WRITE | MSGA_MP_EXEC)
} MSGA_MP;

typedef enum
{
    MSGA_ERR_SYSTEM = -2,
    MSGA_ERR_NOT_IMPLEMENTED = -1,
    MSGA_ERR_OK = 0,
    MSGA_ERR_INVALID_ARGUMENTS,
    MSGA_ERR_INVALID_THUMB_ADDRESS,
    MSGA_ERR_MALLOC_FAIL,
    MSGA_ERR_READ_LENGTH_MISMATCH,
    MSGA_ERR_WRITE_LENGTH_MISMATCH,
    MSGA_ERR_MEMORY_MISMATCH,
    MSGA_ERR_UNKNOWN_INSTRUCTION,
    MSGA_ERR_READ_BACKUP,
    MSGA_ERR_WRITE_ORIGIN,
    MSGA_ERR_READ_STUB,
    MSGA_ERR_WRITE_STUB,
    MSGA_ERR_END
} MSGA_ERR;

#if !defined(NDEBUG)
#include <stdio.h>
#define MSGA_LOG(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#else
#define MSGA_LOG(fmt, ...)
#endif
typedef long long msga_addr_t;
typedef unsigned long long msga_uaddr_t;

typedef MSGA_ERR (*msga_mprotect_t)(msga_addr_t addr, int len, MSGA_MP prot, void *user);
typedef msga_addr_t (*msga_mmap_t)(msga_addr_t addr, int len, MSGA_MP prot, void *user);
typedef MSGA_ERR (*msga_munmap_t)(msga_addr_t addr, int len, void *user);
typedef int (*msga_read_t)(msga_addr_t addr, void *data, int len, void *user);
typedef int (*msga_write_t)(msga_addr_t addr, void const *data, int len, void *user);

typedef struct
{
    msga_mprotect_t mprotect;
    msga_mmap_t mmap;
    msga_munmap_t munmap;
    msga_read_t read;
    msga_write_t write;
    void *user;
} msga_context_t;

typedef struct
{
    msga_context_t *context;
    int backup_len, origin_len, jmpbuf_len, buf_len, cmpbuf_len;
    msga_addr_t target_addr;
    msga_addr_t new_addr;
    msga_addr_t old_addr;
    msga_addr_t origin_addr;

    unsigned char *buffer; // freeable,=backup_len+jmpbuf_len+cmpbuf_len;
    unsigned char *backup; // unfreeable
    unsigned char *jmpbuf; // unfreeable
    unsigned char *cmpbuf; // unfreeable
    unsigned char _buffer[MSGA_BUFFER_SIZE];
} msga_hook_t;

_extern MSGA_ERR msga_init(msga_context_t *ctx, void *user);
_extern MSGA_ERR msga_hook_alloc(msga_hook_t *hook, int backup_len, int jmpbuf_len, int jmpback_len);
_extern MSGA_ERR msga_hook_free(msga_hook_t *hook, int keep_hook);
_extern MSGA_ERR msga_hook_init(msga_hook_t *hook, msga_context_t *ctx, msga_addr_t target_addr, msga_addr_t new_addr, msga_addr_t old_addr);
_extern MSGA_ERR msga_dohook(msga_hook_t *hook);
_extern MSGA_ERR msga_unhook(msga_hook_t *hook);
_extern MSGA_ERR msga_mprotect(msga_context_t *ctx, msga_addr_t addr, int len, MSGA_MP prot);
_extern msga_addr_t msga_mmap(msga_context_t *ctx, msga_addr_t addr, int len, MSGA_MP prot);
_extern MSGA_ERR msga_munmap(msga_context_t *ctx, msga_addr_t addr, int len);
_extern int msga_read(msga_context_t *ctx, msga_addr_t addr, void *data, int len);
_extern int msga_write(msga_context_t *ctx, msga_addr_t addr, void const *data, int len);
_extern char const *msga_error(MSGA_ERR err);
_extern void msga_hook_debug(msga_hook_t *hook);

_extern MSGA_ERR msga_hook_setup(msga_hook_t *hook);
_extern MSGA_ERR msga_hook_setup_x86(msga_hook_t *hook);
_extern MSGA_ERR msga_hook_setup_x64(msga_hook_t *hook);
_extern MSGA_ERR msga_hook_setup_aarch32(msga_hook_t *hook);
_extern MSGA_ERR msga_hook_setup_aarch64(msga_hook_t *hook);
_extern MSGA_ERR msga_hook_setup_thumb(msga_hook_t *hook);

// #ifdef __cplusplus

// #endif

#endif //_MSGA_H
