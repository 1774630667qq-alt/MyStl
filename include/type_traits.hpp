#pragma once
#include <cstddef>

namespace MyStl {

    // 使用 struct，默认全是 public
    template<typename T, T v>
    struct integral_constant {
        // 1. 核心常量
        static constexpr T value = v;

        // 2. 类型别名
        using value_type = T;
        using type = integral_constant<T, v>; // 指向自己的类型

        // 3. 转换函数 (把对象转成值)
        // 语法特点：无返回类型声明
        constexpr operator value_type() const noexcept {
            return value;
        }

        // 4. 仿函数 (像函数一样被调用)
        // 语法特点：必须有返回类型声明！
        constexpr value_type operator()() const noexcept {
            return value;
        }
    };

    // 别名定义
    using true_type = integral_constant<bool, true>;
    using false_type = integral_constant<bool, false>;

    template<typename T> struct remove_const { 
        using type = T; 
    };
    
    template<typename T> struct remove_const<const T> { 
        using type = T; 
    };

    template<typename T>
    using remove_const_t = typename remove_const<T>::type;

    template<typename T> struct remove_volatile {
        using type = T;
    };

    template<typename T> struct remove_volatile<volatile T> {
        using type = T;
    };

    template<typename T>
    using remove_volatile_t = typename remove_volatile<T>::type;

    template<typename T> struct remove_cv {
        using type = remove_const_t<remove_volatile_t<T>>;
    };

    template<typename T>
    using remove_cv_t = typename remove_cv<T>::type;

    template<typename T, typename U>
    struct is_same : false_type {};
    template<typename T, typename U>
    inline constexpr bool is_same_v = is_same<T, U>::value;

    template<typename T>
    struct is_same<T, T> : true_type {};

    template<typename T> struct is_void: is_same<void, remove_cv_t<T>> {}; 
    template<typename T> 
    inline constexpr bool is_void_v = is_void<T>::value;


    template<bool B, typename T = void>
    struct enable_if {};
    template<bool B, typename T = void>
    using enable_if_t = typename enable_if<B, T>::type;

    template<typename T>
    struct enable_if<true, T> {
        using type = T;
    };

    template<bool B, typename A1, typename A2>
    struct conditional {
        using type = A2;
    };
    template<bool B, typename A1, typename A2>
    using conditional_t = typename conditional<B, A1, A2>::type;

    template<typename A1, typename A2>
    struct conditional<true, A1, A2> {
        using type = A1;
    };

    template<typename T>
    struct remove_reference {
        using type = T;
    };
    template<typename T>
    using remove_reference_t = typename remove_reference<T>::type;

    template<typename T>
    struct remove_reference<T&> {
        using type = T;
    };

    template<typename T>
    struct remove_reference<T&&> {
        using type = T;
    };

    template<typename T>
    struct is_lvalue_reference : false_type {};
    template<typename T>
    inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

    template<typename T>
    struct is_lvalue_reference<T&> : true_type {};

    template<typename T>
    struct add_lvalue_reference {
        using type = conditional_t<MyStl::is_void_v<T>, T, T&>;
    };

    template<typename T>
    struct add_lvalue_reference<T&> {
        using type = MyStl::remove_reference_t<T>&;
    };

    template<typename T>
    using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

    template<typename T>
    struct is_rvalue_reference : false_type {};
    template<typename T>
    inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

    template<typename T>
    struct is_rvalue_reference<T&&> : true_type {};

    template<typename T>
    struct add_rvalue_reference {
        using type = conditional_t<MyStl::is_void_v<T>, T, T&&>;
    };

    template<typename T>
    struct add_rvalue_reference<T&&> {
        using type = MyStl::remove_reference_t<T>&&;
    };

    template<typename T>
    using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

    template<typename T>
    struct is_array : false_type {};

    template<typename T> 
    struct is_array<T[]> : true_type {};

    template<typename T, std::size_t N>
    struct is_array<T[N]> : true_type {};
    template<typename T>
    inline constexpr bool is_array_v = is_array<T>::value;

    template<typename T>
    struct remove_extent {
        using type = T;
    };

    template<typename T>
    struct remove_extent<T[]> {
        using type = T;
    };

    template<typename T, std::size_t N>
    struct remove_extent<T[N]> {
        using type = T;
    };

    template<typename T>
    using remove_extent_t = typename remove_extent<T>::type;

    template<typename T>
    struct add_pointer {
        using type = remove_reference_t<T>*;
    };
    
    template<typename T>
    using add_pointer_t = typename add_pointer<T>::type;

    template<typename T>
    struct is_pointer : false_type {};

    template<typename T>
    struct is_pointer<T*> : true_type {};

    template<typename T>
    inline constexpr bool is_pointer_v = is_pointer<T>::value;

    template<typename T>
    struct is_function : false_type {};

    template<typename Ret, typename... Args>
    struct is_function<Ret(Args...)> : true_type {};

    template<typename Ret, typename... Args>
    struct is_function<Ret(Args..., ...)> : true_type {};

    template<typename T>
    inline constexpr bool is_function_v = is_function<T>::value;

    template<typename T>
    struct decay {
        using non_reference_type = remove_reference_t<T>;
        using type = conditional_t<is_function_v<non_reference_type>, add_pointer_t<non_reference_type>, 
            conditional_t<is_array_v<non_reference_type>, add_pointer_t<remove_extent_t<non_reference_type>>, 
                remove_cv_t<non_reference_type>>>;  
    };

    template<typename T>
    using decay_t = typename decay<T>::type;

    template<typename T>
    struct is_trivially_copyable: integral_constant<bool, __is_trivially_copyable(T)> {};
    template<typename T>
    inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;

    template<typename T>
    struct is_trivially_move_constructible: integral_constant<bool, __is_trivially_constructible(T, T&&)> {};
    template<typename T>
    inline constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<T>::value;

    template<typename T>
    struct is_trivially_destructible: integral_constant<bool, __has_trivial_destructor(T)> {};
    template<typename T>
    inline constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;

    template<typename T>
    struct is_nothrow_move_constructible: integral_constant<bool, noexcept(T(MyStl::decay_t<T>()))> {};
    template<typename T>
    inline constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;
} 