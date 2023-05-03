// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#pragma once

#include "types.h"
#include "arch.h"
#include "io.h"
#include "code.h"
#include <map>
#include <list>
#include <tuple>
namespace msga{
    
    class manager{
        arch::base&m_arch;
        io::base&m_io;
        struct hook_t
        {
            addr_t from_addr;
            addr_t to_addr;
            addr_t co_addr;
            addr_t origin_addr;
            msga::code origin;
            msga::code code;
            msga::code backup;
            msga::code code_ptr;
        };
        std::map<addr_t,hook_t>m_map;
        bool dohook(hook_t&hook);
        bool unhook(hook_t&hook);
        public:
        manager(arch::base&arch,io::base&io):m_arch(arch),m_io(io){}
        bool dohook(addr_t from,addr_t to,addr_t origin=0);
        bool unhook(addr_t from,addr_t to);

        template<class T>
        bool dohook(T*from,T*to,T** origin=nullptr){
            return dohook(reinterpret_cast<addr_t>(from),
                        reinterpret_cast<addr_t>(to),
                        reinterpret_cast<addr_t>(origin));
        }
        template<class T>
        bool unhook(T*from,T*to){
            return unhook(reinterpret_cast<addr_t>(from),
                        reinterpret_cast<addr_t>(to));
        }


        io::base&io(){return m_io;};
        arch::base&arch(){return m_arch;};
    };
}