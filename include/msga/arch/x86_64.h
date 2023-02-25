// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#pragma once
#include "msga/arch.h"
namespace msga{
    namespace x86_64{
        enum class opcode:int{
            JO8=0x70,
            JO16=0x800F,
            JO32=0x800F,
            JNO8=0x71,
            JNO16=0x810F,
            JNO32=0x810F,
            JB8=0x72,
            JB16=0x820F,
            JB32=0x820F,
            JC8=0x72,
            JC16=0x820F,
            JC32=0x820F,
            JNAE8=0x72,
            JNAE16=0x820F,
            JNAE32=0x820F,
            JNB8=0x73,
            JNB16=0x830F,
            JNB32=0x830F,
            JNC8=0x73,
            JNC16=0x830F,
            JNC32=0x830F,
            JAE8=0x73,
            JAE16=0x830F,
            JAE32=0x830F,
            JZ8=0x74,
            JZ16=0x840F,
            JZ32=0x840F,
            JE8=0x74,
            JE16=0x840F,
            JE32=0x840F,
            JNZ8=0x75,
            JNZ16=0x850F,
            JNZ32=0x850F,
            JNE8=0x75,
            JNE16=0x850F,
            JNE32=0x850F,
            JBE8=0x76,
            JBE16=0x860F,
            JBE32=0x860F,
            JNA8=0x76,
            JNA16=0x860F,
            JNA32=0x860F,
            JNBE8=0x77,
            JNBE16=0x870F,
            JNBE32=0x870F,
            JA8=0x77,
            JA16=0x870F,
            JA32=0x870F,
            JS8=0x78,
            JS16=0x880F,
            JS32=0x880F,
            JNS8=0x79,
            JNS16=0x890F,
            JNS32=0x890F,
            JP8=0x7A,
            JP16=0x8A0F,
            JP32=0x8A0F,
            JPE8=0x7A,
            JPE16=0x8A0F,
            JPE32=0x8A0F,
            JNP8=0x7B,
            JNP16=0x8B0F,
            JNP32=0x8B0F,
            JPO8=0x7B,
            JPO16=0x8B0F,
            JPO32=0x8B0F,
            JL8=0x7C,
            JL16=0x8C0F,
            JL32=0x8C0F,
            JNGE8=0x7C,
            JNGE16=0x8C0F,
            JNGE32=0x8C0F,
            JNL8=0x7D,
            JNL16=0x8D0F,
            JNL32=0x8D0F,
            JGE8=0x7D,
            JGE16=0x8D0F,
            JGE32=0x8D0F,
            JLE8=0x7E,
            JLE16=0x8E0F,
            JLE32=0x8E0F,
            JNG8=0x7E,
            JNG16=0x8E0F,
            JNG32=0x8E0F,
            JNLE8=0x7F,
            JNLE16=0x8F0F,
            JNLE32=0x8F0F,
            JG8=0x7F,
            JG16=0x8F0F,
            JG32=0x8F0F,
            JCXZ8=0xE3,
            JMP8=0xEB,
            JMP16=0xE9,
            JMP32=0xE9,
            JMPFAR=0xFF,
            JMPFAR_MODRM=0x25,
            CALL32=0xE8,
            CALLFAR=0xFF,
            CALLFAR_MODRM=0x15
           
        };
        bool operator==(int val,opcode op);
   
#pragma pack(push,1)
        struct JCC32
        {
            uint8_t opcode;
            uint8_t opcode2;
            uint32_t offset;
        };
        struct JCC8
        {
            uint8_t opcode;
            uint8_t offset;
        };
        struct JMP32
        {
            uint8_t opcode=static_cast<int>(x86_64::opcode::JMP32);
            uint32_t offset;
        };
        struct JMP8
        {
            uint8_t opcode=static_cast<int>(x86_64::opcode::JMP8);
            uint8_t offset;
        };
        struct JMPFAR
        {
            uint8_t opcode=static_cast<int>(x86_64::opcode::JMPFAR);
            uint8_t modrm=static_cast<int>(x86_64::opcode::JMPFAR_MODRM);
            uint32_t offset;
        };
        struct CALL32
        {
            uint8_t opcode=static_cast<int>(x86_64::opcode::CALL32);
            uint32_t offset;
        };
        struct CALLFAR
        {
            uint8_t opcode=static_cast<int>(x86_64::opcode::CALLFAR);
            uint8_t modrm=static_cast<int>(x86_64::opcode::CALLFAR_MODRM);;
            uint32_t offset;
        };
        struct JMP64
        {
            uint8_t opcode=static_cast<int>(x86_64::opcode::JMPFAR);
            uint8_t modrm=static_cast<int>(x86_64::opcode::JMPFAR_MODRM);
            uint32_t offset=0;
            uint64_t address;
        };
#pragma pack(pop)


    };
    namespace arch{
        class x86_64:public base{
            public:
            // std::vector<uint8_t> request(io::base&io,addr_t addr);
            virtual bool generate(io::base&io,addr_t addr,addr_t to,msga::code&origin,msga::code&generated,msga::code&backup,msga::code&co_ptr,size_t co_size=0) override;
        };
    }
}