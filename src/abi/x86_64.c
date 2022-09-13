// Copyright (c) 2022 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <malloc.h>
#include <string.h>

#include "msga.h"
#include "../utils.h"
#include "x86_64.h"

typedef int (*disasm_width_t)(void *code);

#define JMP8_LEN 2
#define JMP32R_LEN 5
#define JMP32_LEN 10
#define JMP64_LEN 14

#define _JMP_LEN(off, x64) ((off)-JMP8_LEN < (0x80LL) ? JMP8_LEN : (off)-JMP32R_LEN < 0x80000000LL ? JMP32R_LEN \
                                                               : x64                               ? JMP64_LEN  \
                                                                                                   : JMP32_LEN)

#define JMP_LEN(off, x64) _JMP_LEN(off < 0 ? -off : off, x64)

#define CH(off, index) ((((msga_uaddr_t)(off)) >> (index * 8)) & 0xFF)
#define _JMP8(off) \
    {              \
        0xEB, off  \
    }
#define JMP8(off) _JMP8((off - 2))
#define _JMP32R(off)                                         \
    {                                                        \
        0xE9, CH(off, 0), CH(off, 1), CH(off, 2), CH(off, 3) \
    }
#define JMP32R(off) _JMP32R((off - 5))

#define _JMP32(off, addr)                                               \
    {                                                                   \
        0xFF, 0x25, CH(addr, 0), CH(addr, 1), CH(addr, 2), CH(addr, 3), \
            CH(off, 0), CH(off, 1), CH(off, 2), CH(off, 3)              \
    }
#define JMP32(off, addr) _JMP32(off, addr + 6)

#define _JMP64(off)                                         \
    {                                                       \
        0xFF, 0x25, 0, 0, 0, 0,                             \
            CH(off, 0), CH(off, 1), CH(off, 2), CH(off, 3), \
            CH(off, 4), CH(off, 5), CH(off, 6), CH(off, 7)  \
    }
#define JMP64(off) _JMP64(off)

static int make_jmp(void *data, msga_addr_t from, msga_addr_t to, int x64)
{
    const msga_addr_t off = to - from;
    const int len = JMP_LEN(off, x64);

    if (len == JMP8_LEN)
    {
        unsigned char buf[] = JMP8(off);
        memcpy(data, buf, sizeof(buf));
        return MSGA_ERR_OK;
    }
    else if (len == JMP32R_LEN)
    {
        unsigned char buf[] = JMP32R(off);
        memcpy(data, buf, sizeof(buf));
        return MSGA_ERR_OK;
    }
    else if (len == JMP32_LEN)
    {
        unsigned char buf[] = JMP32(to, from);
        memcpy(data, buf, sizeof(buf));
        return MSGA_ERR_OK;
    }
    else if (len == JMP64_LEN)
    {
        unsigned char buf[] = JMP64(to);
        memcpy(data, buf, sizeof(buf));
        return MSGA_ERR_OK;
    }
    return MSGA_ERR_NOT_IMPLEMENTED;
}

static MSGA_ERR _msga_hook_x86_64(msga_context_t *ctx, msga_hook_t *hook, msga_addr_t origin_addr, int isx64)
{
    msga_addr_t off_jmpto = hook->new_addr - hook->target_addr;
    hook->jmp_len = JMP_LEN(off_jmpto, isx64);
    int index = 0;
    unsigned char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    do
    {
        if (index > sizeof(buffer) / 2)
        {
            index = 0;
        }
        if (!index)
        {
            MSGA_TEST(msga_read(ctx, hook->target_addr + hook->backup_len, buffer, sizeof(buffer)) == sizeof(buffer), MSGA_ERR_READ_LENGTH_MISMATCH);
        }
        int width = isx64 ? disasm_width64(buffer + index) : disasm_width32(buffer + index);
        if (width <= 0)
        {
            return MSGA_ERR_UNKNOWN_INSTRUCTION;
        }
        hook->backup_len += width;
        index += width;
    } while (hook->backup_len < hook->jmp_len);

    hook->jmp_len = hook->backup_len;
    MSGA_CHECK(msga_hook_alloc(hook));

    // JMPTO INST.
    memset(hook->jmpbuf, 0x90, hook->jmp_len);
    MSGA_CHECK(make_jmp(hook->jmpbuf, hook->target_addr, hook->new_addr, isx64));

    // ORIGIN INST.
    MSGA_TEST(msga_read(ctx, hook->target_addr, hook->backup, hook->backup_len) == hook->backup_len, MSGA_ERR_BACKUP);
    MSGA_TEST(msga_write(ctx, hook->origin_addr, hook->backup, hook->backup_len) == hook->backup_len, MSGA_ERR_WRITE_LENGTH_MISMATCH);

    // JMPBACK INST.
    msga_addr_t off_jmpback = hook->target_addr - hook->origin_addr;
    int jmpback_len = JMP_LEN(off_jmpback, isx64);
    MSGA_CHECK(make_jmp(buffer, hook->origin_addr + hook->backup_len, hook->target_addr + hook->backup_len, isx64));
    MSGA_TEST(msga_write(ctx, hook->origin_addr + hook->backup_len, buffer, jmpback_len) == jmpback_len, MSGA_ERR_WRITE_LENGTH_MISMATCH);

    if (origin_addr)
    {
        MSGA_TEST(msga_write(ctx, origin_addr, &hook->origin_addr, isx64 ? 8 : 4) == (isx64 ? 8 : 4), MSGA_ERR_WRITE_LENGTH_MISMATCH);
    }
    return MSGA_ERR_OK;
}

MSGA_ERR msga_hook_x86_64(msga_context_t *ctx, msga_hook_t *hook, msga_addr_t target_addr, msga_addr_t new_addr, msga_addr_t origin_addr, int isx64)
{
    memset(hook, 0, sizeof(msga_hook_t));
    hook->context = ctx;
    hook->new_addr = new_addr;
    hook->target_addr = target_addr;

    MSGA_ERR err = _msga_hook_x86_64(ctx, hook, origin_addr, isx64);
    if (err != MSGA_ERR_OK)
    {
        msga_hook_free(hook, 0);
    }
    return err;
}