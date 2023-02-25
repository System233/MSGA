// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#pragma once
#include "io.h"
#include "code.h"
#include <vector>
namespace msga{
    namespace arch{
        class base{
            public:
            virtual bool generate(io::base&io,addr_t addr,addr_t to,msga::code&origin,msga::code&generated,msga::code&backup,msga::code&co_ptr,size_t co_size=0)=0;
        };
    }
}