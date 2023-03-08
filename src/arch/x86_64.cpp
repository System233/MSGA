// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "msga/config.h"
#include "msga/arch/x86_64.h"
#include "msga/code.h"
#include "ihde.h"

#define JMP32_SIZE 5
#define JMP64_SIZE 14
#define JMPFAR_SIZE 6
#define JCC8_SIZE 2
#define JCC32_SIZE 2
#include <stdio.h>
#include <map>

namespace msga{
    namespace x86_64{
        bool operator==(int val,opcode op)
        {
            return (opcode)val==op;
        }
    }


    namespace arch{
        void make_jmp(io::mode mode,msga::code&co, addr_t from,addr_t to){
            if(io::mode::x64==mode){
                msga::x86_64::JMPFAR jmp;
                jmp.offset=0;
                co.write(jmp);
                co.write<uint64_t>(+to);
                co.setaddr(co.size()-sizeof(uint64_t),e_rel::e_rela);
            }else{
                msga::x86_64::JMP32 jmp;
                jmp.offset=to-from-co.size()-sizeof(jmp);
                co.write(jmp);
                co.rebase<decltype(jmp.offset)>(co.size()-sizeof(jmp.offset));
            }
        }
        bool x86_64::generate(io::base&io,addr_t addr,addr_t to,msga::code&origin,msga::code&co,msga::code&backup,msga::code&co_ptr,size_t co_size){
            ihde hde(io.arch());
            uint8_t buffer[MSGA_BUFFER_SIZE];
            size_t off_addr=0,offset=0;
            
            std::map<size_t,size_t>jmp_map;
            std::vector<std::pair<size_t,size_t>>jmp_rel;
            std::vector<std::pair<size_t,addr_t>>jmp_abs;
            io.read(&buffer,addr,MSGA_BUFFER_SIZE);
            
            co.setbase(addr);
            if(!co_size){
                co_size=io.arch()==io::mode::x64?JMP64_SIZE:JMP32_SIZE;
            }
            addr_t bottom=addr+co_size;
            while(off_addr<co_size){
                size_t const width=hde.disasm(buffer+offset);
                addr_t const cur_addr=addr+off_addr;
                jmp_map.emplace(off_addr,co.size());
                if(hde.opcode==0xEB){
                    //JMP OFF8
                    auto off=int8_t(hde.imm.imm8);
                    auto jmpto=cur_addr+width+off;
                    if(msga::io::mode::x64==io.arch()&&(jmpto<addr||jmpto>=bottom)){
                        msga::x86_64::JMPFAR jmp;
                        jmp.offset=sizeof(jmp);
                        co.write(jmp);
                        jmp_abs.emplace_back(co.size()-4,jmpto);
                    }else{
                        msga::x86_64::JMP32 jmp;
                        jmp.opcode=hde.opcode;
                        jmp.offset=jmpto-addr-co.size()-width;
                        co.write(jmp);
                        jmp_rel.emplace_back(co.size()-4,co.size());
                    }
                }else if(hde.opcode>=0x70&&hde.opcode<0x80
                        ||hde.opcode==0x0F&&hde.opcode2>=0x80&&hde.opcode2<=0x8E){
                    //JCC off8,JCC OFF32
                    int32_t off=0;
                    uint8_t opcode=0;
                    if(hde.opcode!=0x0F){
                        opcode=hde.opcode;
                        off=int8_t(hde.imm.imm8);
                    }else{
                        opcode=hde.opcode2-0x10;
                        off=int32_t(hde.imm.imm32);
                    }
                    addr_t jmpto=cur_addr+width+off;
                    if(msga::io::mode::x64==io.arch()&&(jmpto<addr||jmpto>=bottom)){
                        msga::x86_64::JCC8 jcc8;
                        jcc8.opcode=opcode+(opcode&0x1?-1:1);
                        jcc8.offset=sizeof(msga::x86_64::JMPFAR);
                        co.write(jcc8);
                        msga::x86_64::JMPFAR jmpfar;
                        jmpfar.offset=sizeof(jmpfar);
                        co.write(jmpfar);
                        jmp_abs.emplace_back(co.size()-4,jmpto);
                    }else{
                        msga::x86_64::JCC32 jcc;
                        jcc.opcode=0x0F;
                        jcc.opcode2=opcode+0x10;
                        jcc.offset=jmpto-addr-co.size()-sizeof(jcc);
                        co.write(jcc);
                        jmp_rel.emplace_back(co.size()-4,co.size());
                    }
                }else{
                    if(io.arch()==msga::io::mode::x64){
                        if(hde.opcode==0xE8||hde.opcode==0xE9){
                            auto off=int32_t(hde.imm.imm32);
                            auto jmpto=cur_addr+width+off;
                            if(io.mode()==io::mode::x64&&(jmpto<addr||jmpto>=bottom)){
                                msga::x86_64::JMPFAR jmp;
                                jmp.modrm=static_cast<uint8_t>(hde.opcode==0xE8?msga::x86_64::opcode::CALLFAR_MODRM:msga::x86_64::opcode::JMPFAR_MODRM);
                                jmp.offset=sizeof(jmp);
                                co.write(jmp);
                                jmp_abs.emplace_back(co.size()-4,jmpto);
                            }else{
                                msga::x86_64::JMP32 jmp;
                                jmp.opcode=hde.opcode;
                                jmp.offset=jmpto-addr-co.size()-width;
                                co.write(jmp);
                                jmp_rel.emplace_back(co.size()-4,co.size());
                            }
                        }else if(hde.modrm_rm==0x5&&hde.modrm_mod==0&&hde.flags&ihde::flag::DISP32){
                            //INST OFF32
                            auto off=int32_t(hde.disp.disp32);
                            auto jmpto=cur_addr+width+off;
                            auto offaddr=int32_t(jmpto-addr-co.size()-width);
                            auto op_len=width-hde.imm_byte()-4;
                            co.write(&buffer[offset],op_len);
                            co.write(offaddr);
                            co.write(&buffer[offset+op_len+4],hde.imm_byte());
                            jmp_rel.emplace_back(co.size()-4-hde.imm_byte(),co.size());
                        }else{
                            co.write(&buffer[offset],width);
                        }
                    }else if(hde.opcode==0xE8||hde.opcode==0xE9){
                        auto off=int32_t(hde.imm.imm32);
                        addr_t jmpto=cur_addr+width+off;
                        msga::x86_64::JMP32 jmp;
                        jmp.opcode=hde.opcode;
                        jmp.offset=jmpto-addr-co.size()-width;
                        co.write(jmp);
                        jmp_rel.emplace_back(co.size()-4,co.size());
                    }else{
                        co.write(&buffer[offset],width);
                    }
                }
                
                off_addr+=width;
                offset+=width;
                if(offset>(MSGA_BUFFER_SIZE>>1)){
                    io.read(&buffer,addr+off_addr,MSGA_BUFFER_SIZE);
                    offset=0;
                }
            }
            {
                //JMP BACK
                if(io::mode::x64==io.arch()&&io::mode::x64==io.mode()){
                    msga::x86_64::JMPFAR jmp;
                    jmp.offset=sizeof(jmp);
                    co.write(jmp);
                    jmp_abs.emplace_back(co.size()-4,addr+off_addr);
                }else{
                    msga::x86_64::JMP32 jmp;
                    jmp.offset=off_addr-co.size()-sizeof(jmp);
                    co.write(jmp);
                    jmp_rel.emplace_back(co.size()-4,co.size());
                }
            }
            co.resize(aligned(co.size(),static_cast<int>(io.arch())));

            for(auto&item:jmp_rel){
                auto off=co.get<uint32_t>(item.first);
                auto jmpto=off+item.second;
                co.rebase<uint32_t>(item.first);
                if(jmp_map.count(jmpto)){
                    auto new_addr=jmp_map[jmpto];
                    co.set<uint32_t>(item.first,off+(new_addr-jmpto));
                }
            }
            for(auto&item:jmp_abs){
                uint32_t inst_len=co.get<uint32_t>(item.first);
                size_t from_addr=item.first+sizeof(uint32_t);
                size_t to_addr=co.size();
                co.set<uint32_t>(item.first,to_addr-from_addr);
                if(io.arch()==io::mode::x64){
                    co.write<uint64_t>((uint64_t)item.second);
                    co.setaddr(co.size()-sizeof(uint64_t),e_rel::e_rela);
                }else{
                    co.write<uint32_t>(item.second);
                    co.setaddr(co.size()-sizeof(uint32_t),e_rel::e_rela);
                }
            }
            
   
            backup.setbase(addr);
            backup.resize(off_addr);
            io.read(backup);

            if(to){
                //JMP TO
                origin.setbase(addr);
                make_jmp(io.mode(),origin,addr,to);
                origin.resize(off_addr);
            }
            //ORIG CALL
            co_ptr.setbase(0);
            if(io::mode::x64==io.arch()){
                co_ptr.write<uint64_t>(0);
                co_ptr.rebase<uint64_t>(0);
            }else{
                co_ptr.write<uint32_t>(0);
                co_ptr.rebase<uint32_t>(0);
            }
            co_ptr.setaddr(0,e_rel::e_rela);
            
            return true;
        };

    };
}