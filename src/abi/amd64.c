/**
 * Copyright (c) 2022 System233
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <hde64.h>
#include "msga.h"
#include "x86_64.h"

int disasm_width64(void *code)
{
    hde64s hde;
    int width = hde64_disasm(code, &hde);
    if (hde.flags & F_ERROR)
    {
        return -1;
    }
    return width;
}

MSGA_ERR msga_hook_setup_x64(msga_hook_t *hook)
{
    return msga_hook_x86_64(hook, 1);
}