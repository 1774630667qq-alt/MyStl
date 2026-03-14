/*
 * @Author: Zhang YuHua 1774630667@qq.com
 * @Date: 2026-03-14 20:17:21
 * @LastEditors: Zhang YuHua 1774630667@qq.com
 * @LastEditTime: 2026-03-14 20:27:36
 * @FilePath: /MyStl/include/tuple.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include <cstddef>
#include "utility.hpp"

namespace MyStl {

    // ========================================================
    // 第一部分：Tuple 的递归类定义
    // ========================================================

    // 1. 前置声明，接受任意数量、任意类型的模板参数
    template <typename... Values>
    class tuple;

    // 2. 递归的终点：特化一个空的 tuple<>
    template <>
    class tuple<> {
    public:
        tuple() = default;
    };

    // 3. 递归的主体：从参数包中剥离出第一个类型 (Head) 和剩下的类型 (Tail...)
    template <typename Head, typename... Tail>
    class tuple<Head, Tail...> : public tuple<Tail...> {
    private:
        using Base = tuple<Tail...>; // 把剩下的类型打包成基类
        Head head_;                  // 当前这层只存第一个元素

    public:
        // 构造函数：初始化自己 (head_)，并将剩下的参数传给基类 (Base)
        tuple() = default;
        
        tuple(const Head& head, const Tail&... tail) 
            : Base(tail...), head_(head) {}

        // TODO: 请你补充拷贝构造、移动构造和对应的赋值运算符 (注意要同时处理 head_ 和 Base)
        tuple(const tuple& other) 
            : Base(other), head_(other.head_) {}
        
        tuple(tuple&& other) noexcept 
            : Base(MyStl::move(other)), head_(MyStl::move(other.head_)) {}

        tuple& operator=(const tuple& other) {
            if (this != &other) {
                Base::operator=(other);
                head_ = other.head_;
            }
            return *this;
        }

        tuple& operator=(tuple&& other) noexcept {
            if (this != &other) {
                Base::operator=(MyStl::move(other));
                head_ = MyStl::move(other.head_);
            }
            return *this;
        }

        // 获取当前层的数据
        Head& head() { return head_; }
        const Head& head() const { return head_; }

        // 获取基类 (也就是剩下的 tail 数据)
        Base& tail() { return *this; }
        const Base& tail() const { return *this; }
    };


    // ========================================================
    // 第二部分：编译期萃取器 (Tuple Element)
    // 作用：根据索引 I，推导出第 I 个元素的类型，以及它到底在哪一层
    // ========================================================

    // 前置声明
    template <size_t I, typename Tuple>
    struct tuple_element;

    // 递归终点：索引为 0 时，我们要找的类型就是当前层的 Head
    template <typename Head, typename... Tail>
    struct tuple_element<0, tuple<Head, Tail...>> {
        using type = Head;
        using TupleType = tuple<Head, Tail...>; // 记录目标数据所在的具体的基类层级
    };

    // 递归主体：索引不为 0 时，层数 I-1，去 Tail 里面继续找
    template <size_t I, typename Head, typename... Tail>
    struct tuple_element<I, tuple<Head, Tail...>> {
        using type = typename tuple_element<I - 1, tuple<Tail...>>::type;
        using TupleType = typename tuple_element<I - 1, tuple<Tail...>>::TupleType;
    };


    // ========================================================
    // 第三部分：全局魔法函数 get<I>(tuple)
    // ========================================================
    
    // 你的 TODO: 实现 get 魔法函数
    // 提示：
    // 1. 使用 tuple_element<I, tuple<Types...>> 推导出它在哪个层级 (TupleType)
    // 2. 将传入的 t，通过 static_cast 强转为那个层级的基类引用 (TupleType&)
    // 3. 调用该基类的 .head() 方法返回数据！
    template <size_t I, typename... Types>
    typename tuple_element<I, tuple<Types...>>::type& get(tuple<Types...>& t) {
        using TargetTuple = typename tuple_element<I, tuple<Types...>>::TupleType;
        return static_cast<TargetTuple&>(t).head();
    }

    template <size_t I, typename... Types>
    const typename tuple_element<I, tuple<Types...>>::type& get(const tuple<Types...>& t) {
        using TargetTuple = typename tuple_element<I, tuple<Types...>>::TupleType;
        return static_cast<const TargetTuple&>(t).head();
    }

} // namespace MyStl