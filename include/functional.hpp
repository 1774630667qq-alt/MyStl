/*
 * @Author: Zhang YuHua 1774630667@qq.com
 * @Date: 2026-03-02 17:56:46
 * @LastEditors: Zhang YuHua 1774630667@qq.com
 * @LastEditTime: 2026-03-12 18:52:59
 * @FilePath: /MyStl/include/functional.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include <cstddef>
#include <string>

namespace MyStl {
    template<typename T> struct hash;

    template<typename T>
    struct hash<T*> {
        size_t operator()(T* ptr) const noexcept {
            return reinterpret_cast<size_t>(ptr);
        }
    };

    // 使用宏定义一个通用的 hash 模板特化
    #define HASH_PATTERN(Type) \
    template<> \
    struct hash<Type> { \
        size_t operator()(const Type& value) const noexcept { \
            return static_cast<size_t>(value); \
        } \
    };
    
    // 为常用类型定义 hash 特化
    HASH_PATTERN(int)
    HASH_PATTERN(unsigned int)
    HASH_PATTERN(short)
    HASH_PATTERN(unsigned short)
    HASH_PATTERN(long)
    HASH_PATTERN(unsigned long)
    HASH_PATTERN(bool)
    HASH_PATTERN(char) // 对ASCII进行哈希
    HASH_PATTERN(unsigned char)
    HASH_PATTERN(wchar_t)
    HASH_PATTERN(signed char)
    HASH_PATTERN(char16_t)
    HASH_PATTERN(char32_t)
    HASH_PATTERN(long long)
    HASH_PATTERN(unsigned long long)

    template<>
    struct hash<float> {
        size_t operator()(float value) const noexcept {
            return *reinterpret_cast<const size_t*>(&value);
        }
    };

    template<>
    struct hash<double> {
        size_t operator()(double value) const noexcept {
            return *reinterpret_cast<const long long*>(&value);
        }
    };

    template<>
    struct hash<long double> {
        size_t operator()(long double value) const noexcept {
            return *reinterpret_cast<const long long*>(&value);
        }
    };

    template<>
    struct hash<const char*> { 
        size_t operator()(const char* str) const noexcept { 
            unsigned long long OffsetBasis = 14695981039346656037ULL;
            unsigned long long Prime = 1099511628211ULL;
            size_t hashValue = OffsetBasis; 
            while (*str) { 
                hashValue ^= static_cast<unsigned char>(*str); 
                hashValue *= Prime; 
                ++str; 
            }
            return hashValue;
        } 
    };

    template<typename T>
    struct less {
        constexpr bool operator()(const T& lhs, const T& rhs) const {
            return lhs < rhs;
        }
    };

    template<typename T>
    struct greater {
        constexpr bool operator()(const T& lhs, const T& rhs) const {
            return lhs > rhs;   
        }
    };

    template <typename T>
    struct identity {
        const T& operator()(const T& x) const {
            return x;
        }
    };

    template <typename Pair>
    struct select1st {
        const typename Pair::first_type& operator()(const Pair& x) const {
            return x.first;
        }
    };
}