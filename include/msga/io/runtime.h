// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "msga/io.h"

namespace msga
{
    namespace io{
        class runtime:public base{
        public:
            virtual msga::io::mode arch()const override;
            virtual msga::io::mode mode()const override;
            virtual void read(void*data,addr_t addr,size_t len)override;
            void write(void const*data, addr_t addr,size_t len)override;
            virtual addr_t alloc(size_t len,addr_t preferred=0)override;
            virtual void free(addr_t addr,size_t len)override;
        };
    }
}