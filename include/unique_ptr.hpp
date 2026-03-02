#pragma once
#include <cstddef>
#include "type_traits.hpp"
#include "utility.hpp"

namespace MyStl {
    template<typename T>
    class unique_ptr {
    public:
        using ValueType = T;
        using Pointer = ValueType*;
        using Reference = ValueType&;
    private:
        Pointer ptr;
    public:
        unique_ptr() : ptr(nullptr) {}
        explicit unique_ptr(Pointer ptr) : ptr(ptr) {}
        unique_ptr(std::nullptr_t) : ptr(nullptr) {}

        // 1. 禁止拷贝构造和拷贝赋值（核心：防止多重所有权）
        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        // 2. 移动构造函数（核心：转移所有权）
        unique_ptr(unique_ptr&& other) noexcept : ptr(other.ptr) {
            other.ptr = nullptr; 
        }

        unique_ptr& operator=(std::nullptr_t) {
            delete ptr;
            ptr = nullptr;
            return *this;
        }

        Pointer release() {
            Pointer temp = ptr;
            ptr = nullptr;
            return temp;
        }

        void reset(Pointer p = nullptr) {
            // 关键：防止自重置导致的 Double Free
            Pointer old_ptr = ptr;
            ptr = p;
            if (old_ptr) delete old_ptr; 
        }

        explicit operator bool() const {
            return ptr != nullptr;
        }

        void swap(unique_ptr& other) noexcept {
            MyStl::swap(ptr, other.ptr);
        }

        unique_ptr& operator=(unique_ptr&& other) noexcept {
            if (this != &other) {
                swap(other); 
            }
            return *this;
        }

        // 主模板（单对象）：提供 * 和 -> 重载
        Reference operator*() const {
            return *ptr;
        }

        Pointer operator->() const {
            return ptr;
        }

        Pointer get() const {
            return ptr;
        }

        ~unique_ptr() {
            delete ptr;
        }
    };

    // 针对数组类型的偏特化：unique_ptr<T[]>
    template<typename T>
    class unique_ptr<T[]> {
    public:
        using ValueType = T;
        using Pointer = ValueType*;
        using Reference = ValueType&;
    private:
        Pointer ptr;
    public:
        unique_ptr() : ptr(nullptr) {}
        explicit unique_ptr(Pointer ptr) : ptr(ptr) {}
        unique_ptr(std::nullptr_t) : ptr(nullptr) {}

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        unique_ptr(unique_ptr&& other) noexcept : ptr(other.ptr) {
            other.ptr = nullptr;
        }

        Pointer release() {
            Pointer temp = ptr;
            ptr = nullptr;
            return temp;
        }

        void reset(Pointer p = nullptr) {
            // 关键：防止自重置导致的 Double Free
            Pointer old_ptr = ptr;
            ptr = p;
            if (old_ptr) delete[] old_ptr; 
        }

        explicit operator bool() const {
            return ptr != nullptr;
        }

        void swap(unique_ptr& other) noexcept {
            MyStl::swap(ptr, other.ptr);
        }

        unique_ptr& operator=(unique_ptr&& other) noexcept {
            if (this != &other) {
                swap(other);
            }
            return *this;
        }

        unique_ptr& operator=(std::nullptr_t) {
            delete[] ptr;
            ptr = nullptr;
            return *this;
        }

        // 数组特化：提供 [] 重载
        Reference operator[](int index) const {
            return ptr[index];
        }

        Pointer get() const {
            return ptr;
        }

        ~unique_ptr() {
            delete[] ptr; // 核心区别：使用 delete[]
        }
    };

    template<typename T>
    void swap(unique_ptr<T>& lhs, unique_ptr<T>& rhs) noexcept {
        lhs.swap(rhs);
    }

    // 针对数组版本的重载
    template<typename T>
    void swap(unique_ptr<T[]>& lhs, unique_ptr<T[]>& rhs) noexcept {
        lhs.swap(rhs);
    }

    // 1. 针对非数组类型 (Single Object)
    template<typename T, typename... Args>
    typename MyStl::enable_if<!MyStl::is_array<T>::value, unique_ptr<T>>::type
    make_unique(Args&&... args) {
        return unique_ptr<T>(new T(MyStl::forward<Args>(args)...));
    }

    // 2. 针对数组类型 (Array)
    template<typename T>
    typename MyStl::enable_if<MyStl::is_array<T>::value, unique_ptr<T>>::type
    make_unique(std::size_t size) {
        using U = typename MyStl::remove_extent<T>::type;
        return unique_ptr<T>(new U[size]());
    }
}