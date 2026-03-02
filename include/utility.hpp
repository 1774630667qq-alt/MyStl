#pragma once
#include "type_traits.hpp"

namespace MyStl {
    // move
    template<typename T>
    constexpr typename MyStl::remove_reference_t<T>&& move(T&& arg) noexcept {
        return static_cast<typename MyStl::remove_reference_t<T>&&>(arg);
    }

    // forward (左值版本)
    template<typename T>
    constexpr T&& forward(typename MyStl::remove_reference_t<T>& arg) noexcept {
        return static_cast<T&&>(arg);
    }

    // forward (右值版本)
    template<typename T>
    constexpr T&& forward(typename MyStl::remove_reference_t<T>&& arg) noexcept {
        return static_cast<T&&>(arg);
    }

    // swap
    // [优化] 加上 noexcept 规范，如果 move 是不抛异常的，swap 也不抛异常
    template<typename T>
    void swap(T& a, T& b) noexcept(
        MyStl::is_nothrow_move_constructible_v<T> && 
        noexcept(a = MyStl::move(b)) // 这里简化了，严谨应该检测 is_nothrow_move_assignable
    ) {
        T temp = MyStl::move(a);
        a = MyStl::move(b);
        b = MyStl::move(temp);
    }
    template <typename T1, typename T2>
    struct pair {
        using first_type  = T1;
        using second_type = T2;

        T1 first;
        T2 second;

        // 默认构造
        constexpr pair() : first(), second() {}

        // 拷贝构造 (传入 const 引用)
        constexpr pair(const T1& a, const T2& b) : first(a), second(b) {}

        // 万能引用构造 (支持移动语义，非常重要！)
        template <typename U1, typename U2>
        constexpr pair(U1&& a, U2&& b) 
            : first(MyStl::forward<U1>(a)), second(MyStl::forward<U2>(b)) {}
            
        // 允许从另一个 pair 拷贝/移动 (简化版)
        template <typename U1, typename U2>
        constexpr pair(const pair<U1, U2>& p) : first(p.first), second(p.second) {}
    };

    // 2. MyStl::make_pair 的定义
    // 这是一个非常方便的语法糖，利用了函数模板参数的自动推导功能
    // 这样你写 make_pair(1, "hello") 时就不用写 pair<int, const char*>(...) 了
    template <typename T1, typename T2>
    constexpr pair<T1, T2> make_pair(T1&& a, T2&& b) {
        return pair<T1, T2>(MyStl::forward<T1>(a), MyStl::forward<T2>(b));
    }
}