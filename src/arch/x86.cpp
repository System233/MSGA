// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "msga/config.h"
#include "msga/arch/x86_64.h"
#include "msga/arch/x86.h"
#include "hde32.h"

#define ABS_JMP_SIZE 5


namespace msga{
    namespace arch{
        bool x86::generate(io::base&io,addr_t addr,std::vector<code>&generated,std::vector<code>&backup,addr_t origin=0){
            hde32s hde;
            uint8_t buffer[MSGA_BUFFER_SIZE];
            size_t off_addr=0,offset=0;
            io.read(&buffer,addr,MSGA_BUFFER_SIZE);
            code co;
            std::vector<std::pair<size_t,size_t>>record;
            auto get_increase_size=[&record](size_t off){
                size_t size=0;
                for(auto it=record.rbegin();it!=record.rend();++it){
                    size+=it->second;
                    if(it->first==off){
                        break;
                    }
                }
                return size;
            };
            while(off_addr<ABS_JMP_SIZE){
                size_t width=hde32_disasm(buffer+offset,&hde);
                size_t last_size=co.size();
                if(hde.opcode>=0x70&&hde.opcode<0x80||hde.opcode==x86_64::opcode::JMP8){
                    int8_t off8=hde.imm.imm8;
                    addr_t jmpto=addr+off_addr+off8+width;
                    if(jmpto<addr){
                        if(hde.opcode==x86_64::opcode::JMP8){
                            msga::x86_64::JMP32 jmp32;
                            jmp32.offset=jmpto-sizeof(jmp32);
                            co.setrel_imm(co.size()-4);
                        }else{
                            msga::x86_64::JCC32 jcc32;
                            jcc32.opcode=0x0F;
                            jcc32.opcode2=hde.opcode+0x10;
                            jcc32.offset=jmpto-sizeof(jcc32);
                            co.write(jcc32);
                            co.setrel_imm(co.size()-4);
                        }
                    }else if(jmpto-addr<off_addr){
                        size_t inc=get_increase_size(jmpto-addr);
                        if(inc){
                            msga::x86_64::JCC8 jcc8;
                            jcc8.opcode=hde.opcode;
                            jcc8.offset=off8-inc;
                            co.write(jcc8);
                        }else{
                            co.write(&buffer[offset],width);
                        }
                    }else{
                        co.write(&buffer[offset],width);
                    }
                }if(hde.opcode==0x0F&&hde.opcode2>=0x80&&hde.opcode2<=0x70){
                    int32_t off32=hde.imm.imm32;
                    addr_t jmpto=addr+off_addr+off32+width;
                    msga::x86_64::JCC32 jcc32;
                    jcc32.opcode=hde.opcode;
                    jcc32.opcode2=hde.opcode2;
                    jcc32.offset=jmpto-sizeof(jcc32);
                    co.write(jcc32);
                    co.setrel_imm(co.size()-4);
                }else if(hde.opcode==x86_64::opcode::JMP32){
                    int32_t off32=hde.imm.imm32;
                    addr_t jmpto=addr+off_addr+off32+width;
                    msga::x86_64::JMP32 jmp32;
                    jmp32.offset=jmpto-sizeof(jmp32);
                    co.setrel_imm(co.size()-4);
                }else{
                    co.write(&buffer[offset],width);
                }
                record.emplace_back(off_addr,co.size()-last_size);
                off_addr+=width;
                offset+=width;
                if(offset>(MSGA_BUFFER_SIZE>>1)){
                    io.read(&buffer,addr+off_addr,MSGA_BUFFER_SIZE);
                    offset=0;
                }
            }


            code bak(addr,off_addr);
            io.read(bak);
            backup.push_back(bak);
            generated.push_back(co);
        };

    };
}