/*
 * @Author: Zhang YuHua 1774630667@qq.com
 * @Date: 2026-03-14 21:09:15
 * @LastEditors: Zhang YuHua 1774630667@qq.com
 * @LastEditTime: 2026-03-14 22:50:28
 * @FilePath: /MyStl/include/algorithm.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include "iterator.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include <cstring>

namespace MyStl{
    /**
     * @brief 在指定范围内查找第一个等于给定值的元素。
     * 
     * @tparam InputIterator 输入迭代器类型
     * @tparam T 要查找的值的类型
     * @param first 查找范围的起始迭代器
     * @param last 查找范围的结束迭代器
     * @param value 要查找的值
     * @return InputIterator 指向找到的元素的迭代器。如果未找到，则返回 last。
     */
    template<typename InputIterator, typename T>
    InputIterator find (InputIterator first, InputIterator last, const T& value) {
        for (; first != last; ++first) {
            if (*first == value) {
                return first;
            }
        }
        return last;
    }

    /**
     * @brief 在指定范围内查找第一个满足谓词条件的元素。
     * 
     * @tparam InputIterator 输入迭代器类型
     * @tparam Predicate 一元谓词函数类型
     * @param first 查找范围的起始迭代器
     * @param last 查找范围的结束迭代器
     * @param pred 一元谓词（接受一个参数并返回 bool 的可调用对象）
     * @return InputIterator 指向第一个满足条件的元素的迭代器。如果未找到，则返回 last。
     */
    template <typename InputIterator, typename Predicate>
    InputIterator find_if(InputIterator first, InputIterator last, Predicate pred) {
        for (; first != last; ++first) {
            if (pred(*first)) {
                return first;
            }
        }
        return last;
    }

    /**
     * @brief 将指定范围内的所有元素设置为给定值。
     * 
     * @tparam ForwardIterator 前向迭代器类型
     * @tparam T 填充值的类型
     * @param first 填充范围的起始迭代器
     * @param last 填充范围的结束迭代器
     * @param value 用于填充的具体值
     */
    template <typename ForwardIterator, typename T>
    void fill(ForwardIterator first, ForwardIterator last, const T& value) {
        for (; first != last; ++first) {
            *first = value;
        }
    }

    /**
     * @brief 将一个范围内的元素复制到另一个范围。
     *        针对支持内存拷贝优化的类型（如原生指针且元素可以不抛异常地移动/拷贝）进行了特化加速，直接调用 memmove。
     * 
     * @tparam InputIterator 源区间的输入迭代器类型
     * @tparam OutputIterator 目标区间的输出迭代器类型
     * @param first 源范围的起始迭代器
     * @param last 源范围的结束迭代器
     * @param result 目标范围的起始迭代器
     * @return OutputIterator 指向目标范围内最后一个被复制元素的下一个位置的迭代器(尾部迭代器)
     */
    template<typename InputIterator, typename OutputIterator>
    OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {  
        using T = typename MyStl::iterator_traits<OutputIterator>::value_type;  
        
        if constexpr (MyStl::is_pointer_v<InputIterator> && 
                      MyStl::is_pointer_v<OutputIterator> &&
                      MyStl::is_trivially_copy_assignable_v<T>) {
            
            size_t n = last - first; 
            if (n > 0) {
                std::memmove(result, first, sizeof(T) * n);
            }
            
            return result + n; 
        } else {
            for (; first != last; ++first, ++result) {
                *result = *first;
            }
            return result;
        }
    }

    /**
     * @brief 反转指定范围内的元素顺序。
     * 
     * @tparam BidirectionalIterator 双向迭代器类型
     * @param first 范围的起始迭代器
     * @param last 范围的结束迭代器
     */
    template <typename BidirectionalIterator>
    void reverse(BidirectionalIterator first, BidirectionalIterator last) {
        while (first != last && first != --last) {
            MyStl::swap(*first, *last);
            ++first;
        }
    }

    /**
     * @brief 移除指定范围内所有等于给定值的元素。
     *        注意：算法并不会改变容器的实际大小，而是将不等于 value 的元素前移，
     *        并返回新的逻辑结尾的迭代器。并且保证剩余元素的相对顺序不变。
     * 
     * @tparam ForwardIterator 前向迭代器类型
     * @tparam T 要移除的值的类型
     * @param first 范围的起始迭代器
     * @param last 范围的结束迭代器
     * @param value 要移除的值
     * @return ForwardIterator 指向移除操作后新的逻辑结尾的迭代器
     */
    template <typename ForwardIterator, typename T>
    ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T& value) {
        ForwardIterator result = first;
        for (; first != last; ++first) {
            if (!(*first == value)) {
                if (result != first) { // 避免不必要的自我赋值
                    *result = MyStl::move(*first);
                }
                ++result;
            }
        }
        return result;
    }

    /**
     * @brief 移除指定范围内相邻的重复元素。
     *        注意：通常需要先对范围进行排序，以便将相同的元素聚在一起。
     *        算法不会改变容器大小，而是将去重后的元素移到前部。
     * 
     * @tparam ForwardIterator 前向迭代器类型
     * @param first 范围的起始迭代器
     * @param last 范围的结束迭代器
     * @return ForwardIterator 指向去重后新的逻辑结尾的迭代器
     */
    template <typename ForwardIterator>
    ForwardIterator unique(ForwardIterator first, ForwardIterator last) {
        if (first == last) {
            return last;
        }
        ForwardIterator result = first;
        while (++first != last) {
            if (!(*result == *first)) {
                *(++result) = MyStl::move(*first);
            }
        }
        return ++result;
    }
}