// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "types.h"
#include <vector>
#include <map>

namespace msga{

    class code{
    public:
        struct rebase_t{
            virtual void rebase(msga::code&co,size_t offset,diff_t diff)const=0;
            virtual size_t size()const=0;
        };
    protected:
        template<class T>
        struct relocation:public rebase_t
        {
            size_t size()const override{return sizeof(T);}
            void rebase(msga::code&co,size_t offset,diff_t diff)const override{
                T&addr=co.get<T>(offset);
                addr-=diff;
            }
            // static relocation<T>& get(){return value;};
            static relocation<T> value;
        };
        
        addr_t m_base;
        std::vector<uint8_t>m_data;
        std::map<size_t,rebase_t*>m_rebase;
        std::vector<size_t>m_addr_table;
        
    public:
        code(addr_t base=0,size_t size=0):m_base(base),m_data(size,0){}
        size_t size()const{return m_data.size();}
        uint8_t const*get()const{return m_data.data();}
        uint8_t*get(){return m_data.data();}
        void write(uint8_t*ptr,size_t len){
            m_data.insert(m_data.end(),ptr,ptr+len);
        }
        template<class T>
        void write(T&&data){
            write((uint8_t*)&data,sizeof(T));
        }
        addr_t base()const{return m_base;}
        auto const& data()const{return m_data;}

        void setaddr(size_t offset){
            m_addr_table.emplace_back(offset);
        }
        auto const&getaddr()const{
            return m_addr_table;
        }
        void setbase(addr_t new_base){
            diff_t diff=new_base-base();
            for(auto&item:m_rebase){
                item.second->rebase(*this,item.first,diff);
            }
            m_base=new_base;
        }
        void resize(size_t new_size){
            m_data.resize(new_size);
        }
        template<class T>
        void rebase(size_t offset){
            m_rebase[offset]=&relocation<T>::value;
        }

        template<class T>
        T&get(size_t off){
            return *(T*)&m_data[off];
        }
        template<class T>
        void set(size_t off,T &&val){
            *((T*)&m_data[off])=std::forward<T>(val);
        }
        operator bool()const{return size();}
        
        
    };
    
    template<class T>
    code::relocation<T> code::relocation<T>::value;
}