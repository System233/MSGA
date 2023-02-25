// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "../arch.h"
namespace msga{
    namespace arch{
        class x86:public base{
            public:
            virtual bool generate(io::base&io,addr_t addr,std::vector<code>&generated,std::vector<code>&backup,addr_t origin=0);
        };
    }
}