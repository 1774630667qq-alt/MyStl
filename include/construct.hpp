#pragma once
#include "iterator.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

namespace MyStl {
    template<typename T> 
    void construct(T* ptr) {
        ::new (static_cast<void*>(ptr)) T();
    }

    template<typename T, typename... Args> 
    void construct(T* ptr, Args&&... args) {
        ::new (static_cast<void*>(ptr)) T(MyStl::forward<Args>(args)...);
    }

    template<typename T>
    void destroy(T* ptr) {
        ptr->~T();
    }

    // 针对 Trivial 类型：直接什么都不做
    template<typename ForwardIterator>
    void destroy_cat(ForwardIterator , ForwardIterator , MyStl::true_type) {}

    // 针对非 Trivial 类型：循环调用析构
    template<typename ForwardIterator>
    void destroy_cat(ForwardIterator first, ForwardIterator last, MyStl::false_type) {
        for (; first != last; ++first) {
            destroy(&*first); // 只有在这里才真正取地址
        }
    }

    template<typename ForwardIterator>
    void destroy(ForwardIterator first, ForwardIterator last) {
        // 使用 iterator_traits 获取值类型
        using T = typename MyStl::iterator_traits<ForwardIterator>::value_type;
        // 根据是否可以忽略析构过程进行分流
        destroy_cat(first, last, typename MyStl::is_trivially_destructible<T>::type());
    }
}