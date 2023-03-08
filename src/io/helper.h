// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include <istream>
#include <ostream>
#include <vector>

template <class T>
struct raw
{
    T &data;
    raw(T &data) : data(data) {}
};

template <class T>
std::istream &operator>>(std::istream &is, raw<T> &&data)
{
    return is.read(reinterpret_cast<char *>(&data.data), sizeof(T));
}
template <class T>
std::ostream &operator<<(std::ostream &is, raw<T> const&data)
{
    return is.write(reinterpret_cast<char *>(&data.data), sizeof(T));
}

template <class T>
std::istream &operator>>(std::istream &is, T &data)
{
    static_assert(std::is_trivially_copyable<T>::value, "T is not trivially copyable");
    return is.read(reinterpret_cast<char *>(&data), sizeof(T));
}
template <class T>
std::ostream &operator<<(std::ostream &is, T const&data)
{
    static_assert(std::is_trivially_copyable<T>::value, "T is not trivially copyable");
    return is.write(reinterpret_cast<char const*>(&data), sizeof(T));
}

template <class T>
std::istream &operator>>(std::istream &is, std::vector<T> &data)
{
    return is.read(reinterpret_cast<char *>(data.data()), sizeof(T) * data.size());
}
template <class T>
std::ostream &operator<<(std::ostream &os, std::vector<T> const&data)
{
    return os.write(reinterpret_cast<char const*>(data.data()), sizeof(T) * data.size());
}
#include<string>

template <class T>
std::vector<uint8_t> &operator<<(std::vector<uint8_t> &data, T const&value)
{
    static_assert(std::is_trivially_copyable<T>::value, "T is not trivially copyable");
    auto begin=reinterpret_cast<uint8_t const*>(&value);
    data.insert(data.end(), begin, begin + sizeof(T));
    return data;
}
std::vector<uint8_t> &operator<<(std::vector<uint8_t> &data, std::string const&value)
{
    auto begin=reinterpret_cast<uint8_t const*>(&value[0]);
    data.insert(data.end(), begin, begin + value.size()+1);
    return data;
}