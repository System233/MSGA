// Copyright (c) 2023 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <cstdint>
#include "msga/io.h"

class ihde
{
public:
    enum class flag : uint32_t
    {
        MODRM = 0x00000001,
        SIB = 0x00000002,
        IMM8 = 0x00000004,
        IMM16 = 0x00000008,
        IMM32 = 0x00000010,
        IMM64 = 0x00000020,
        DISP8 = 0x00000040,
        DISP16 = 0x00000080,
        DISP32 = 0x00000100,
    };

    union
    {
        uint8_t imm8;
        uint16_t imm16;
        uint32_t imm32;
        uint64_t imm64;
    } imm;
    union
    {
        uint8_t disp8;
        uint16_t disp16;
        uint32_t disp32;
    } disp;
    uint8_t opcode;
    uint8_t opcode2;
    // union{
    //     uint8_t mod:2;
    //     uint8_t reg:3;
    //     uint8_t rm:3;
    // }modrm;
    uint8_t modrm;
    uint8_t modrm_mod;
    uint8_t modrm_reg;
    uint8_t modrm_rm;
    flag flags;
    msga::io::mode mode;
    ihde(msga::io::mode mo) : mode(mo) {}
    size_t disasm(void *buffer)
    {
        if (mode == msga::io::mode::x64)
        {
            return disasm64(buffer);
        }
        return disasm32(buffer);
    };
    size_t disasm32(void *buffer);
    size_t disasm64(void *buffer);
    size_t imm_byte() const;

    size_t disp_byte() const;
};

inline bool operator&(ihde::flag const &x, ihde::flag const &y)
{
    return static_cast<uint32_t>(x) & static_cast<uint32_t>(y);
};
