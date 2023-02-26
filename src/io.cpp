// Copyright (c) 2023 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "msga/io.h"

namespace msga
{
    namespace io
    {

        void *allocate(uint8_t *data, size_t size, size_t request, size_t align)
        {
            if(request==0){
                return nullptr;
            }
            request = aligned(request, align) + sizeof(uint32_t);
            size_t offset = 0;
            auto&init_size = *reinterpret_cast<uint32_t *>(data);
            if(init_size==0){
                init_size=size;
            }
            while (offset + sizeof(uint32_t) < size)
            {
                auto &cur_size = *reinterpret_cast<uint32_t *>(data + offset);
                if (cur_size & 0x80000000)
                {
                    offset += cur_size & 0x7FFFFFFF;
                    continue;
                }
                while (offset + cur_size + sizeof(uint32_t) <= cur_size)
                {
                    auto next_size = *reinterpret_cast<uint32_t *>(data + offset + cur_size);
                    if (next_size & 0x80000000)
                    {
                        break;
                    }
                    cur_size += next_size;
                }
                auto addr = &data[offset + sizeof(uint32_t)];
                auto next_size = cur_size - request;
                *reinterpret_cast<uint32_t *>(data + offset + request) = next_size;
                cur_size = request | 0x80000000;
                return addr;
            }
            return nullptr;
        }

        void deallocate(void *addr)
        {
            auto &size = *reinterpret_cast<uint32_t *>(((uint8_t *)addr) - sizeof(uint32_t));
            size &= 0x7FFFFFFF;
        }
    }
}