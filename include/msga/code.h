// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "types.h"
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
namespace msga{
    enum e_rel{
        e_rel_sys=-1,
        e_rela=0,
        e_rel8=1,
        e_rel16=2,
        e_rel32=4,
        e_rel64=8,
    };
    class rel_base_t{
        size_t m_offset;
        e_rel m_type;
        public:
        rel_base_t(size_t off,e_rel type):m_offset(off),m_type(type){}
        e_rel type()const{return m_type;};
        size_t offset()const{return m_offset;};
        virtual ~rel_base_t(){}
    };
    template<class T>
    class rel_sys_t:public rel_base_t{
        T m_data;
        public:
        rel_sys_t(size_t off,T const&data):rel_base_t(off,e_rel::e_rel_sys),m_data(data){}
        T const&data()const{return m_data;}
    };
    using rel_list_t=std::vector<std::unique_ptr<rel_base_t>>;
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
        
        std::vector<std::unique_ptr<rel_base_t>>m_addr_table;
        void check_offset(size_t offset){
            if(offset>=size()){
                throw std::length_error("offset>=code.size()");
            }
        }
    public:
        code(addr_t base=0,size_t size=0):m_base(base),m_data(size,0){}
        void moveto(addr_t new_base){m_base=new_base;}
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

        void setaddr(size_t offset,e_rel type){
            check_offset(offset);
            m_addr_table.emplace_back(std::make_unique<rel_base_t>(offset,type));
        }
        auto const&getaddr()const{
            return m_addr_table;
        }
        auto&getaddr(){
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
            check_offset(offset);
            m_rebase[offset]=&relocation<T>::value;
        }

        template<class T>
        T&get(size_t offset){
            check_offset(offset);
            return *(T*)&m_data[offset];
        }
        template<class T>
        void set(size_t offset,T &&val){
            get<T>(offset)=std::forward<T>(val);
        }
        operator bool()const{return size();}
        
        
    };
    
    template<class T>
    code::relocation<T> code::relocation<T>::value;
}