// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "helper.h"
template <>
std::vector<uint8_t> &operator<<(std::vector<uint8_t> &data, std::string const&value)
{
    auto begin=reinterpret_cast<uint8_t const*>(&value[0]);
    data.insert(data.end(), begin, begin + value.size()+1);
    return data;
}