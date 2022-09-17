// Copyright (c) 2022 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <string.h>
#include "msga.h"
#include "../utils.h"

//ADR IP1, #12
//LDR IP1,[IP1]
//BR IP1
//<addr 64>

#define JMP_LEN 20
#define MAKE_JMPBUF(addr) { \
    0x71,0x00,0x00,0x10,    \
    0x31,0x02,0x40,0xF9,    \
    0x20,0x02,0x1F,0xD6,    \
    LB8(addr) \
}

MSGA_ERR msga_hook_setup_aarch64(msga_hook_t *hook)
{
    MSGA_ERROR_BEGIN;
    if (hook->target_addr & 1)
    {
        return msga_hook_setup_thumb(hook);
    }
    MSGA_CHECK(msga_hook_alloc(hook, JMP_LEN, JMP_LEN, JMP_LEN));
    unsigned char jmpbuf[JMP_LEN] = MAKE_JMPBUF(hook->new_addr);
    unsigned char jmpback_buf[JMP_LEN] = MAKE_JMPBUF(hook->target_addr+hook->backup_len);
    memcpy(hook->jmpbuf, jmpbuf, sizeof(jmpbuf));
    MSGA_TEST(msga_read(hook->context, hook->target_addr, hook->backup, hook->backup_len) == hook->backup_len, MSGA_ERR_READ_BACKUP);
    MSGA_TEST(msga_write(hook->context, hook->origin_addr, hook->backup, hook->backup_len) == hook->backup_len, MSGA_ERR_WRITE_ORIGIN);
    MSGA_TEST(msga_write(hook->context, hook->origin_addr + hook->backup_len, jmpback_buf, sizeof(jmpback_buf)) == sizeof(jmpback_buf), MSGA_ERR_WRITE_ORIGIN);
    return MSGA_ERR_NOT_IMPLEMENTED;
    MSGA_ERROR_END;
}