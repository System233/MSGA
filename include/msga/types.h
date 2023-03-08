/**
 * Copyright (c) 2023 System233
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>
namespace msga{
    using addr_t=uint64_t;
    using diff_t=int64_t;
    using offset_t=uint64_t;

    // using hook_addr_t=std::pair<addr_t,addr_t>;
    template<class T>
    constexpr T aligned(T value,size_t alignto){
        return (value+alignto-1)&~(alignto-1);
    }
    template<class T>
    constexpr T ceil(T value,size_t alignto){
        return (value+alignto-1)&~(alignto-1);
    }
    template<class T>
    constexpr T floor(T value,size_t alignto){
        return (value)&~(alignto-1);
    }
    template<class T>
    constexpr T congruence_modulo(T t,T x,T mod){
        auto modx=x%mod;
        auto result=t-(t%mod)+modx;
        if(result<t){
            result+=mod;
        }
        return result;
    }
    
}