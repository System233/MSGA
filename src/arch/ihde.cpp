// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "ihde.h"
size_t ihde::imm_byte() const
{
    if (flags & ihde::flag::IMM64)
    {
        return 8;
    }
    if (flags & ihde::flag::IMM32)
    {
        return 4;
    }
    if (flags & ihde::flag::IMM16)
    {
        return 2;
    }
    if (flags & ihde::flag::IMM8)
    {
        return 1;
    }
    return 0;
}
size_t ihde::disp_byte() const
{
    if (flags & ihde::flag::DISP32)
    {
        return 4;
    }
    if (flags & ihde::flag::DISP16)
    {
        return 2;
    }
    if (flags & ihde::flag::DISP8)
    {
        return 1;
    }
    return 0;
}