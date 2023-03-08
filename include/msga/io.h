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
            unknown=0,
            x32=4,
            x64=8,
        };
        

        class base{
            public:
            virtual mode arch()const=0;
            virtual mode mode()const=0;
            virtual void read(void*data,addr_t addr,size_t len)=0;
            virtual void write(void const*data, addr_t addr,size_t len)=0;
            virtual void rebase(addr_t addr,rel_base_t*opt){};
            virtual void debase(addr_t addr){};
            virtual void search_rebase(rel_list_t&items,addr_t addr,size_t len){};
            virtual void range_rebase(rel_list_t const&items,addr_t addr,size_t len){
                for(auto&item:items){
                    rebase(addr,item.get());
                }
            };
            virtual addr_t alloc(size_t len,addr_t preferred=0)=0;
            virtual void free(addr_t addr,size_t len)=0;
            virtual void read(msga::code&co){
                search_rebase(co.getaddr(),co.base(),co.size());
                read(co.get(),co.base(),co.size());
            }
            virtual void write(msga::code const&co){
                write(co.get(),co.base(),co.size());
                range_rebase(co.getaddr(),co.base(),co.size());
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
        
        void* allocate(uint8_t*data,size_t size,size_t request,size_t align);
        void deallocate(void* addr);
    }
}