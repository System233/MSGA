// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include "ihde.h"
#include "hde32.h"
size_t ihde::disasm32(void*buffer){
    hde32s hde;
    int width=hde32_disasm(buffer,&hde);
    opcode=hde.opcode;
    opcode2=hde.opcode2;
    modrm=hde.modrm;
    modrm_mod=hde.modrm_mod;
    modrm_reg=hde.modrm_reg;
    modrm_rm=hde.modrm_rm;
    imm.imm32=hde.imm.imm32;
    disp.disp32=hde.disp.disp32;
    flags=static_cast<flag>((hde.flags&0x1F)|((hde.flags&0xE0)<<1));
    return width;
};