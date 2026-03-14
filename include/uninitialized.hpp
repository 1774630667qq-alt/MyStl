/*
 * @Author: Zhang YuHua 1774630667@qq.com
 * @Date: 2026-03-13 21:49:38
 * @LastEditors: Zhang YuHua 1774630667@qq.com
 * @LastEditTime: 2026-03-14 13:52:29
 * @FilePath: /MyStl/include/uninitialized.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%A
 */
#pragma once
#include "iterator.hpp"
#include "construct.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include <cstddef>
#include "allocator.hpp"

namespace MyStl {
    template<typename InputIterator, typename ForwardIterator>
    ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
        using T = typename MyStl::iterator_traits<ForwardIterator>::value_type;
        using Alloc = MyStl::allocator<T>;
        ForwardIterator cur = result;
        try {
            for (; first != last; ++first, ++cur) {
                Alloc::construct(&*cur, *first);
            }
            return cur;
        } catch (...) {
            for (; result != cur; ++result) Alloc::destroy(&*result);
            throw;
        }
    }

    template<typename ForwardIterator, typename Size, typename T>
    void uninitialized_fill_n(ForwardIterator first, Size n, const T& val) {
        using ValueType = typename MyStl::iterator_traits<ForwardIterator>::value_type;
        using Alloc = MyStl::allocator<ValueType>;
        
        ForwardIterator cur = first;
        try {
            for (; n > 0; --n, ++cur) {
                Alloc::construct(&*cur, val); 
            }
        } catch (...) {
            for (; first != cur; ++first) Alloc::destroy(&*first);
            throw;
        }
    }

    // 去掉无法推导的模板参数，将 T 的获取移到函数体内
    template<typename InputIterator, typename ForwardIterator>
    ForwardIterator uninitialized_move(InputIterator first, InputIterator last, ForwardIterator result) {
        using ValueType = typename MyStl::iterator_traits<ForwardIterator>::value_type;
        using Alloc = MyStl::allocator<ValueType>;
        
        ForwardIterator cur = result;
        try {
            for (; first != last; ++first, ++cur) {
                Alloc::construct(&*cur, MyStl::move(*first)); // 使用 &*cur
            }
            return cur;
        } catch (...) {
            for (ForwardIterator temp = result; temp != cur; ++temp) {
                Alloc::destroy(&*temp); // 使用 &*temp
            }
            throw;
        }
    } 
}