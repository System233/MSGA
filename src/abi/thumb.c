// Copyright (c) 2022 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <string.h>
#include "msga.h"
#include "../utils.h"

// NOP; #00BF
// LDR PC,[PC]
// <addr 32>

#define JMP_LEN 8
#define MAKE_JMPBUF(addr) {0xDF, 0xF8, 0x00, 0xF0, LB4(addr)};

#define ALIGN 4
#define TEST_WIDTH(value) ((value)&0xF800 > 0xE000) ? 4 : 2

MSGA_ERR msga_hook_setup_thumb(msga_hook_t *hook)
{
    MSGA_ERROR_BEGIN;
    msga_addr_t target_addr = hook->target_addr & ~1;

    int padding = MSGA_ALIGN(target_addr, 4) - target_addr;
    MSGA_TEST(padding == 0 || padding == 2, MSGA_ERR_INVALID_THUMB_ADDRESS);

    unsigned char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    int index = 0, backup_len = padding, jmpbuf_len = padding + JMP_LEN;
    while (backup_len < jmpbuf_len)
    {
        if (index > sizeof(buffer) / 2)
        {
            index = 0;
        }
        if (!index)
        {
            MSGA_TEST(msga_read(hook->context, target_addr + backup_len, buffer, sizeof(buffer)) == sizeof(buffer), MSGA_ERR_READ_LENGTH_MISMATCH);
        }
        int width = TEST_WIDTH(*(uint16_t *)&buffer[index]);
        if (width <= 0)
        {
            return MSGA_ERR_UNKNOWN_INSTRUCTION;
        }
        backup_len += width;
        index += width;
    }

    MSGA_CHECK(msga_hook_alloc(hook, backup_len, jmpbuf_len, JMP_LEN + ALIGN));

    msga_addr_t origin_addr = MSGA_ALIGN(hook->origin_addr, 4);

    unsigned char nopbuf[0] = {0x00, 0xBF};
    unsigned char jmpbuf[JMP_LEN] = MAKE_JMPBUF(hook->new_addr);
    unsigned char jmpback_buf[JMP_LEN] = MAKE_JMPBUF(hook->target_addr + hook->backup_len);

    memcpy(hook->jmpbuf, nopbuf, padding);
    memcpy(hook->jmpbuf + padding, jmpbuf, sizeof(jmpbuf));

    MSGA_TEST(msga_read(hook->context, target_addr, hook->backup, hook->backup_len) == hook->backup_len, MSGA_ERR_READ_BACKUP);
    MSGA_TEST(msga_write(hook->context, hook->origin_addr, hook->backup, hook->backup_len) == hook->backup_len, MSGA_ERR_WRITE_ORIGIN);
    MSGA_TEST(msga_write(hook->context, hook->origin_addr + hook->backup_len, jmpback_buf, sizeof(jmpback_buf)) == sizeof(jmpback_buf), MSGA_ERR_WRITE_ORIGIN);
    MSGA_ERROR_END;
}