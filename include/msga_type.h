// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _MSGA_TYPE_H
#define _MSGA_TYPE_H

#ifdef __cplusplus
#define MSGA_EXTERN extern "C" 
#else
#define MSGA_EXTERN
#endif

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

typedef enum {
    MSGA_ARCH_UNK,
    MSGA_ARCH_X86,
    MSGA_ARCH_X64,
    MSGA_ARCH_A32,
    MSGA_ARCH_A64,
    MSGA_ARCH_THUMB,
} MSGA_ARCH;

typedef enum
{
    MSGA_ERR_SYSTEM = -2,
    MSGA_ERR_NOT_IMPLEMENTED = -1,
    MSGA_ERR_OK = 0,
    MSGA_ERR_INVALID_ARGUMENTS,
    MSGA_ERR_INVALID_THUMB_ADDRESS,
    MSGA_ERR_MALLOC_FAIL,
    MSGA_ERR_MEM_RW_FAIL,
    MSGA_ERR_MEMORY_MISMATCH,
    MSGA_ERR_UNKNOWN_INSTRUCTION,
    MSGA_ERR_READ_BACKUP,
    MSGA_ERR_WRITE_ORIGIN,
    MSGA_ERR_READ_STUB,
    MSGA_ERR_WRITE_STUB,
    MSGA_ERR_END
} MSGA_ERR;

// typedef long long msga_addr_t;
typedef unsigned long long msga_addr_t;
typedef unsigned char msga_uint8_t;

#endif //_MSGA_TYPE_H