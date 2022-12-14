// Copyright (c) 2022 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <hde32.h>
#include "msga.h"
#include "x86_64.h"

int disasm_width32(void *code)
{
    hde32s hde;
    int width = hde32_disasm(code, &hde);
    if (hde.flags & F_ERROR)
    {
        return -1;
    }
    return width;
}

MSGA_ERR msga_hook_setup_x86(msga_hook_t *hook)
{
    return msga_hook_x86_64(hook, 0);
}