// Copyright (c) 2022 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MSGA_IMPL_X86_64_H
#define MSGA_IMPL_X86_64_H

#include "msga.h"

int disasm_width32(void *code);
int disasm_width64(void *code);
MSGA_ERR msga_hook_x86_64(msga_hook_t *hook, int isx64);
#endif