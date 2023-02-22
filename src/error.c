/**
 * Copyright (c) 2022 System233
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "msga.h"

#define EK(ERR, MSG) \
    case ERR:        \
        return MSG;

char const *msga_error(MSGA_ERR err)
{
    switch (err)
    {
        EK(MSGA_ERR_OK, "ok");
        EK(MSGA_ERR_MALLOC_FAIL, "malloc fail");
        EK(MSGA_ERR_NOT_IMPLEMENTED, "not implemented");
        EK(MSGA_ERR_MEM_RW_FAIL, "read or write memory fail");
        EK(MSGA_ERR_MEMORY_MISMATCH, "memory data mismatch");
        EK(MSGA_ERR_UNKNOWN_INSTRUCTION, "unknown instruction");
        EK(MSGA_ERR_READ_BACKUP, "read backup fail");
        EK(MSGA_ERR_INVALID_ARGUMENTS, "invalid arguments");
        EK(MSGA_ERR_SYSTEM,"system error");
    default:
        break;
    }
    return "invalid errno";
}
#include <stdio.h>

void msga_hook_debug(msga_hook_t *hook)
{
#define HDEBUG(fmt, name) printf(#name "=" fmt "\n", name);
#define HDEBUGA(name, len)                      \
    do                                          \
    {                                           \
        printf(#name "[%d]=", len);             \
        for (int i = 0; i < len && (name); ++i) \
            printf("%02x,", name[i]);           \
        printf("\n");                           \
    } while (0)
    HDEBUG("%p", hook->context);
    // HDEBUG("%#llx", hook->origin_addr);
    // HDEBUG("%d", hook->origin_len);
    HDEBUG("%#llx", hook->jmp_addr);
    HDEBUG("%#llx", hook->from_addr);
    HDEBUG("%#llx", hook->to_addr);
    // HDEBUGA(hook->backup, hook->backup_len);
    // HDEBUGA(hook->jmpbuf, hook->jmpbuf_len);
}