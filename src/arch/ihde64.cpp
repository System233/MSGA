// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "ihde.h"
#include "hde64.h"
size_t ihde::disasm64(void*buffer){
    hde64s hde;
    int width=hde64_disasm(buffer,&hde);
    opcode=hde.opcode;
    opcode2=hde.opcode2;
    modrm=hde.modrm;
    modrm_mod=hde.modrm_mod;
    modrm_reg=hde.modrm_reg;
    modrm_rm=hde.modrm_rm;
    imm.imm32=hde.imm.imm32;
    disp.disp32=hde.disp.disp32;
    flags=static_cast<flag>(hde.flags&0x1FF);
    return width;
};