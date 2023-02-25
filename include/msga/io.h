/**
 * Copyright (c) 2023 System233
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#pragma once
#include "types.h"
#include "code.h"
namespace msga{
    namespace io{
        enum class mode{
            x32=4,
            x64=8,
        };
        class base{
            public:
            virtual mode arch()const=0;
            virtual mode mode()const=0;
            virtual void read(void*data,addr_t addr,size_t len)=0;
            virtual void write(void const*data, addr_t addr,size_t len)=0;
            virtual void rebase(addr_t addr,size_t len){};

            virtual addr_t alloc(size_t size,addr_t perfer=0)=0;
            virtual void free(addr_t addr)=0;
            virtual void read(msga::code&co){
                read(co.get(),co.base(),co.size());
            }
            virtual void write(msga::code const&co){
                for(size_t off:co.getaddr()){
                    rebase(co.base()+off,static_cast<size_t>(this->arch()));
                }
                write(co.get(),co.base(),co.size());
            }
            

            template<class T>
            void read(T&data,addr_t addr){
                read(&data,addr,sizeof(T));
            }
            template<class T>
            T read(addr_t addr){
                T data;
                read(&data,addr);
                return data;
            }
            template<class T>
            void write(T&data,addr_t addr){
                write(&data,addr,sizeof(T));
            }
        };
    }
}