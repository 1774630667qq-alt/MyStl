#pragma once
#include <cstddef>

namespace MyStl {

    /**
     * @brief 编译期常量包装器
     * 
     * 这是类型萃取（Type Traits）的基础组件。它将一个编译期常量值（v）包装成一个类型。
     * 所有编译期计算的结果通常都继承自这个类（例如 true_type, false_type）。
     * 
     * @tparam T 常量的类型 (如 bool, int)
     * @tparam v 常量的值
     */
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

    /**
     * @brief 移除类型的 const 限定符
     * 
     * 如果类型 T 是 const 限定的，则移除其 const 限定符；
     * 否则保持类型不变。
     * 
     * @tparam T 要处理的类型
     */
    template<typename T> struct remove_const { 
        using type = T; 
    };
    
    template<typename T> struct remove_const<const T> { 
        using type = T; 
    };

    template<typename T>
    using remove_const_t = typename remove_const<T>::type;

    /**
     * @brief 移除类型的 volatile 限定符
     * 
     * 如果类型 T 是 volatile 限定的，则移除其 volatile 限定符；
     * 否则保持类型不变。
     * 
     * @tparam T 要处理的类型
     */
    template<typename T> struct remove_volatile {
        using type = T;
    };

    template<typename T> struct remove_volatile<volatile T> {
        using type = T;
    };

    template<typename T>
    using remove_volatile_t = typename remove_volatile<T>::type;

    /**
     * @brief 移除类型的 const 和 volatile 限定符
     * 
     * 组合使用 remove_const 和 remove_volatile，移除类型的所有 cv 限定符。
     * 例如：const volatile int -> int
     * 
     * @tparam T 要处理的类型
     */
    template<typename T> struct remove_cv {
        using type = remove_const_t<remove_volatile_t<T>>;
    };

    template<typename T>
    using remove_cv_t = typename remove_cv<T>::type;

    /**
     * @brief 判断两个类型是否相同
     * 
     * 如果类型 T 和 U 完全相同，则继承自 true_type；
     * 否则继承自 false_type。
     * 
     * @tparam T 第一个类型
     * @tparam U 第二个类型
     */
    template<typename T, typename U>
    struct is_same : false_type {};
    template<typename T, typename U>
    inline constexpr bool is_same_v = is_same<T, U>::value;

    template<typename T>
    struct is_same<T, T> : true_type {};

    /**
     * @brief 判断类型是否为 void
     * 
     * 检查类型 T 是否为 void（忽略 cv 限定符）。
     * 例如：void, const void, volatile void 都会返回 true。
     * 
     * @tparam T 要检查的类型
     */
    template<typename T> struct is_void: is_same<void, remove_cv_t<T>> {}; 
    template<typename T> 
    inline constexpr bool is_void_v = is_void<T>::value;


    /**
     * @brief 条件类型启用器
     * 
     * SFINAE（Substitution Failure Is Not An Error）的核心工具。
     * 当条件 B 为 true 时，type 成员类型存在且为 T；
     * 当条件 B 为 false 时，type 成员类型不存在，导致模板替换失败。
     * 
     * 常用于模板函数重载，根据类型特性选择不同的实现。
     * 
     * @tparam B 条件布尔值
     * @tparam T 条件为 true 时启用的类型
     */
    template<bool B, typename T = void>
    struct enable_if {};
    template<bool B, typename T = void>
    using enable_if_t = typename enable_if<B, T>::type;

    template<typename T>
    struct enable_if<true, T> {
        using type = T;
    };

    /**
     * @brief 编译期条件类型选择
     * 
     * 根据布尔条件 B 在两个类型之间选择：
     * - 如果 B 为 true，则 type 为 A1
     * - 如果 B 为 false，则 type 为 A2
     * 
     * 类似于三元运算符 ?: 的类型版本。
     * 
     * @tparam B 条件布尔值
     * @tparam A1 条件为 true 时选择的类型
     * @tparam A2 条件为 false 时选择的类型
     */
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

    /**
     * @brief 移除类型的引用
     * 
     * 如果类型 T 是引用类型（左值引用或右值引用），则移除引用；
     * 否则保持类型不变。
     * 例如：int& -> int, int&& -> int, int -> int
     * 
     * @tparam T 要处理的类型
     */
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

    /**
     * @brief 判断类型是否为左值引用
     * 
     * 如果类型 T 是左值引用类型（如 int&），则继承自 true_type；
     * 否则继承自 false_type。
     * 
     * @tparam T 要检查的类型
     */
    template<typename T>
    struct is_lvalue_reference : false_type {};
    template<typename T>
    inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

    template<typename T>
    struct is_lvalue_reference<T&> : true_type {};

    /**
     * @brief 为类型添加左值引用
     * 
     * 如果 T 不是 void 类型，则添加左值引用；
     * 如果 T 已经是引用类型，则先移除原有引用再添加左值引用。
     * 对于 void 类型，保持不变（因为 void& 是非法的）。
     * 
     * @tparam T 要处理的类型
     */
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

    /**
     * @brief 判断类型是否为右值引用
     * 
     * 如果类型 T 是右值引用类型（如 int&&），则继承自 true_type；
     * 否则继承自 false_type。
     * 
     * @tparam T 要检查的类型
     */
    template<typename T>
    struct is_rvalue_reference : false_type {};
    template<typename T>
    inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

    template<typename T>
    struct is_rvalue_reference<T&&> : true_type {};

    /**
     * @brief 为类型添加右值引用
     * 
     * 如果 T 不是 void 类型，则添加右值引用；
     * 如果 T 已经是引用类型，则先移除原有引用再添加右值引用。
     * 对于 void 类型，保持不变（因为 void&& 是非法的）。
     * 
     * @tparam T 要处理的类型
     */
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

    /**
     * @brief 获取类型 T 的右值引用，用于在未求值上下文中生成值
     * 
     * declval 是一个只能在未求值上下文（如 decltype、noexcept、sizeof 等）中使用的函数模板。
     * 它可以在不需要默认构造函数的情况下，获取类型 T 的实例引用。
     * 
     * 典型用途：
     * - 在 decltype 表达式中获取表达式的类型
     * - 在 noexcept 表达式中检查操作是否抛出异常
     * 
     * @note 此函数没有定义，只能在未求值上下文中使用。
     * @tparam T 要获取引用的类型
     * @return T 的右值引用类型
     */
    template<typename T>
    add_rvalue_reference_t<T> declval() noexcept;

    /**
     * @brief 判断类型是否为数组类型
     * 
     * 如果类型 T 是数组类型（包括未知边界数组 T[] 和已知边界数组 T[N]），
     * 则继承自 true_type；否则继承自 false_type。
     * 
     * @tparam T 要检查的类型
     */
    template<typename T>
    struct is_array : false_type {};

    template<typename T> 
    struct is_array<T[]> : true_type {};

    template<typename T, std::size_t N>
    struct is_array<T[N]> : true_type {};
    template<typename T>
    inline constexpr bool is_array_v = is_array<T>::value;

    /**
     * @brief 移除数组类型的最外层维度
     * 
     * 如果类型 T 是数组类型，则移除其最外层数组维度；
     * 否则保持类型不变。
     * 例如：int[3] -> int, int[] -> int, int -> int, int[3][4] -> int[4]
     * 
     * @tparam T 要处理的类型
     */
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

    /**
     * @brief 为类型添加指针
     * 
     * 移除类型的引用后，添加指针。
     * 例如：int -> int*, int& -> int*, const int -> const int*
     * 
     * @tparam T 要处理的类型
     */
    template<typename T>
    struct add_pointer {
        using type = remove_reference_t<T>*;
    };
    
    template<typename T>
    using add_pointer_t = typename add_pointer<T>::type;

    /**
     * @brief 判断类型是否为指针类型
     * 
     * 如果类型 T 是指针类型（如 int*），则继承自 true_type；
     * 否则继承自 false_type。
     * 
     * @note 此特性不识别成员指针或智能指针。
     * @tparam T 要检查的类型
     */
    template<typename T>
    struct is_pointer : false_type {};

    template<typename T>
    struct is_pointer<T*> : true_type {};

    template<typename T>
    inline constexpr bool is_pointer_v = is_pointer<T>::value;

    /**
     * @brief 判断类型是否为函数类型
     * 
     * 如果类型 T 是函数类型，则继承自 true_type；
     * 否则继承自 false_type。
     * 
     * 支持检测普通函数类型和可变参数函数类型。
     * @note 函数指针和成员函数指针不是函数类型。
     * @tparam T 要检查的类型
     */
    template<typename T>
    struct is_function : false_type {};

    template<typename Ret, typename... Args>
    struct is_function<Ret(Args...)> : true_type {};

    template<typename Ret, typename... Args>
    struct is_function<Ret(Args..., ...)> : true_type {};

    template<typename T>
    inline constexpr bool is_function_v = is_function<T>::value;

    /**
     * @brief 模拟函数传值参数时的类型退化规则 (std::decay)
     * 
     * 转换规则：
     * 1. 先移除引用 (remove_reference)。
     * 2. 如果是函数，退化为函数指针 (Function-to-Pointer decay)。
     * 3. 如果是数组，退化为指针 (Array-to-Pointer decay)。
     * 4. 否则，移除顶层的 const/volatile 限定符 (remove_cv)。
     */
    template<typename T>
    struct decay {
        using non_reference_type = remove_reference_t<T>;
        using type = conditional_t<is_function_v<non_reference_type>, add_pointer_t<non_reference_type>, 
            conditional_t<is_array_v<non_reference_type>, add_pointer_t<remove_extent_t<non_reference_type>>, 
                remove_cv_t<non_reference_type>>>;  
    };

    template<typename T>
    using decay_t = typename decay<T>::type;

    /**
     * @brief 检测类型是否可平凡复制
     * 
     * 如果类型可以通过 memcpy 或 memmove 安全地复制（例如没有自定义的拷贝/移动构造函数或析构函数），
     * 则此特性为 true。这对于优化容器操作和序列化非常重要。
     * 
     * @note 依赖编译器内置函数 __is_trivially_copyable。
     * @tparam T 要检查的类型
     */
    template<typename T>
    struct is_trivially_copyable: integral_constant<bool, __is_trivially_copyable(T)> {};
    template<typename T>
    inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;

    /**
     * @brief 检测类型是否具有平凡的移动构造函数
     * 
     * 如果类型的移动构造函数是平凡的（即编译器自动生成且不执行任何自定义操作），
     * 则此特性为 true。平凡移动构造等同于内存复制。
     * 
     * @note 依赖编译器内置函数 __is_trivially_constructible。
     * @tparam T 要检查的类型
     */
    template<typename T>
    struct is_trivially_move_constructible: integral_constant<bool, __is_trivially_constructible(T, T&&)> {};
    template<typename T>
    inline constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<T>::value;

    /**
     * @brief 检测类型是否具有平凡的析构函数
     * 
     * 如果类型的析构函数是平凡的（即编译器自动生成且不执行任何自定义操作），
     * 则此特性为 true。平凡析构函数意味着销毁对象不需要任何额外操作。
     * 
     * @note 依赖编译器内置函数 __has_trivial_destructor。
     * @tparam T 要检查的类型
     */
    template<typename T>
    struct is_trivially_destructible: integral_constant<bool, __has_trivial_destructor(T)> {};
    template<typename T>
    inline constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;

    /**
     * @brief 检测类型是否具有不抛出异常的移动构造函数
     * 
     * 这是一个核心的类型特性检查。它使用 `noexcept` 运算符来判断类型 T 的移动构造过程是否保证不抛出异常。
     * 在 vector 等容器的扩容逻辑中，如果此特性为 true，容器可以使用移动语义来优化性能；
     * 否则为了保证异常安全性（避免在移动一半时抛出异常导致数据丢失），通常会回退到拷贝操作。
     */
    template<typename T>
    struct is_nothrow_move_constructible: integral_constant<bool, noexcept(T(MyStl::declval<T>()))> {};
    template<typename T>
    inline constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;

    /**
     * @brief is_integral 的内部辅助类
     * 
     * 用于判断类型是否为整型。通过模板特化，为所有整型类型提供 true_type，
     * 其他类型默认为 false_type。
     * 
     * @note 这是内部实现细节，用户应使用 is_integral。
     * @tparam T 要检查的类型
     */
    template<typename T> 
    struct is_integral_helper: false_type {};

    template <> struct is_integral_helper<bool>               : true_type {};
    template <> struct is_integral_helper<char>               : true_type {};
    template <> struct is_integral_helper<signed char>        : true_type {};
    template <> struct is_integral_helper<unsigned char>      : true_type {};
    template <> struct is_integral_helper<wchar_t>            : true_type {};
    template <> struct is_integral_helper<char16_t>           : true_type {};
    template <> struct is_integral_helper<char32_t>           : true_type {};
    template <> struct is_integral_helper<short>              : true_type {};
    template <> struct is_integral_helper<unsigned short>     : true_type {};
    template <> struct is_integral_helper<int>                : true_type {};
    template <> struct is_integral_helper<unsigned int>       : true_type {};
    template <> struct is_integral_helper<long>               : true_type {};
    template <> struct is_integral_helper<unsigned long>      : true_type {};
    template <> struct is_integral_helper<long long>          : true_type {};
    template <> struct is_integral_helper<unsigned long long> : true_type {};

    /**
     * @brief 判断类型是否为整型
     * 
     * 检查类型 T 是否为整型（包括 bool、char、short、int、long、long long 及其无符号变体）。
     * 此特性会忽略 cv 限定符，例如 const int 也会返回 true。
     * 
     * @tparam T 要检查的类型
     */
    template<typename T>
    struct is_integral: is_integral_helper<typename remove_cv<T>::type> {};

    template<typename T>
    inline constexpr bool is_integral_v = is_integral<T>::value;

    /**
     * @brief 检测类型是否具有平凡的拷贝赋值运算符
     * 
     * 如果类型具有平凡的拷贝赋值运算符，意味着其赋值操作等同于内存复制（memmove）。
     * @note 依赖编译器内置函数 __is_trivially_copy_assignable。
     */
    template<typename T>
    struct is_trivially_copy_assignable: integral_constant<bool, __is_trivially_copy_assignable(T)> {};
    template<typename T>
    inline constexpr bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<T>::value;
} 