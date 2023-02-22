// Copyright (c) 2022 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <string.h>
#include "msga.h"
#include "../utils.h"


// LDR PC,[PC,#-4]
// <addr 32>

#define JMP_LEN 8
#define MAKE_JMPBUF(addr)  {0x40, 0xF0, 0x1F, 0xE5, LB4(addr)}; 

MSGA_ERR msga_hook_setup_aarch32(msga_hook_t *hook)
{
    MSGA_ERROR_BEGIN;
    if (hook->to_addr & 1)
    {
        return msga_hook_setup_thumb(hook);
    }
    MSGA_CHECK(msga_hook_alloc(hook, JMP_LEN, JMP_LEN, JMP_LEN));
    unsigned char jmpbuf[JMP_LEN] = MAKE_JMPBUF(hook->jmp_addr);
    unsigned char jmpback_buf[JMP_LEN] = MAKE_JMPBUF(hook->to_addr+hook->backup_len);
    memcpy(hook->jmpbuf, jmpbuf, sizeof(jmpbuf));
    MSGA_TEST(msga_read(hook->context, hook->to_addr, hook->backup, hook->backup_len) == hook->backup_len, MSGA_ERR_READ_BACKUP);
    MSGA_TEST(msga_write(hook->context, hook->origin_addr, hook->backup, hook->backup_len) == hook->backup_len, MSGA_ERR_WRITE_ORIGIN);
    MSGA_TEST(msga_write(hook->context, hook->origin_addr + hook->backup_len, jmpback_buf, sizeof(jmpback_buf)) == sizeof(jmpback_buf), MSGA_ERR_WRITE_ORIGIN);
    MSGA_ERROR_END;
}