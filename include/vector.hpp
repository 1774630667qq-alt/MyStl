#pragma once
#include <cstddef>
#include <stdexcept>
#include <initializer_list>
#include "utility.hpp"
#include "type_traits.hpp"
#include "iterator.hpp"
#include "allocator.hpp"

namespace MyStl {

    // 1. 定义 vectorIterator
    // 继承 MyStl::iterator，自动获得 iterator_category 等 5 个类型定义！
    template<typename T, bool IsConst>
    class vectorIterator : public MyStl::iterator<
        MyStl::random_access_iterator_tag, // Tag: 我是随机访问迭代器
        T,                                 // ValueType
        std::ptrdiff_t,                    // DistanceType
        typename MyStl::conditional_t<IsConst, const T*, T*>, // Pointer
        typename MyStl::conditional_t<IsConst, const T&, T&>  // Reference
    > 
    {
    public:
        // 既然继承了，我们就可以直接用父类的类型别名了
        using typename MyStl::iterator<MyStl::random_access_iterator_tag, T, std::ptrdiff_t, 
            typename MyStl::conditional_t<IsConst, const T*, T*>, 
            typename MyStl::conditional_t<IsConst, const T&, T&> >::pointer;
            
        using typename MyStl::iterator<MyStl::random_access_iterator_tag, T, std::ptrdiff_t, 
            typename MyStl::conditional_t<IsConst, const T*, T*>, 
            typename MyStl::conditional_t<IsConst, const T&, T&> >::reference;
            
        // 或者更简单的写法（推荐）：重新声明一下，方便内部使用
        using ptr_type = typename MyStl::conditional_t<IsConst, const T*, T*>;

    private:
        ptr_type ptr; // 使用计算出的指针类型

    public:
        // 构造函数
        vectorIterator(ptr_type p = nullptr) : ptr(p) {}

        // 允许从非 Const 迭代器构造 Const 迭代器 (iterator -> const_iterator)
        // 这是一个非常贴心的功能！
        template<bool OtherConst, typename = MyStl::enable_if_t<IsConst && !OtherConst>>
        vectorIterator(const vectorIterator<T, OtherConst>& other) : ptr(other.base()) {}

        // 获取内部指针（为了反向迭代器或其他用途）
        ptr_type base() const { return ptr; }

        // --- 核心操作 ---
        reference operator*() const { return *ptr; }
        ptr_type operator->() const { return ptr; }
        reference operator[](int n) const { return *(ptr + n); }

        // --- 移动操作 ---
        vectorIterator& operator++() { ++ptr; return *this; }
        vectorIterator operator++(int) { vectorIterator temp = *this; ++ptr; return temp; }
        vectorIterator& operator--() { --ptr; return *this; }
        vectorIterator operator--(int) { vectorIterator temp = *this; --ptr; return temp; }

        vectorIterator& operator+=(int n) { ptr += n; return *this; }
        vectorIterator& operator-=(int n) { ptr -= n; return *this; }
        vectorIterator operator+(int n) const { return vectorIterator(ptr + n); }
        vectorIterator operator-(int n) const { return vectorIterator(ptr - n); }
        
        // 两个迭代器相减，返回 difference_type
        std::ptrdiff_t operator-(const vectorIterator& other) const { return ptr - other.ptr; }

        // --- 比较操作 ---
        bool operator==(const vectorIterator& other) const { return ptr == other.ptr; }
        bool operator!=(const vectorIterator& other) const { return ptr != other.ptr; }
        bool operator<(const vectorIterator& other) const { return ptr < other.ptr; }
        bool operator>(const vectorIterator& other) const { return ptr > other.ptr; }
        bool operator<=(const vectorIterator& other) const { return ptr <= other.ptr; }
        bool operator>=(const vectorIterator& other) const { return ptr >= other.ptr; }
    };
    
    // 全局重载：支持 n + iter 的写法 (比如 5 + it)
    template<typename T, bool IsConst>
    vectorIterator<T, IsConst> operator+(int n, const vectorIterator<T, IsConst>& it) {
        return it + n;
    }

    template<typename T, typename Alloc = MyStl::allocator<T>>
    class vector
    {
        public:
        using ValueType = T;
        using iterator = vectorIterator<T, false>;
        using const_iterator = vectorIterator<T, true>;
        using pointer = T*;
        using reference = T&;
        using const_pointer = const T*;
        using const_reference = const T&;
        using difference_type = std::ptrdiff_t;

        using reverse_iterator = MyStl::reverse_iterator<iterator>;
        using const_reverse_iterator = MyStl::reverse_iterator<const_iterator>;
        private:
        T* Vec;
        int m_size;
        int m_capacity;
        using allocator_type = Alloc;
        public:
        vector();
        vector(int size, const T& value = T());
        vector(std::initializer_list<T> list);
        template<typename inputIterator,
            typename = MyStl::enable_if_t<!MyStl::is_integral_v<inputIterator>>>
        vector(inputIterator first, inputIterator last); 
        vector(const vector& vec);
        vector(vector&& vec);
        ~vector();
        reference operator[](int index);
        const_reference operator[](int index) const;
        iterator begin();
        iterator end();
        reverse_iterator rbegin();
        reverse_iterator rend();
        const_reverse_iterator rbegin() const;
        const_reverse_iterator rend() const;
        const_iterator begin() const;
        const_iterator end() const;
        const_iterator cbegin() const;
        const_iterator cend() const;
        vector& operator=(const vector& vec);
        vector& operator=(vector&& vec) noexcept;
        vector& operator=(std::initializer_list<T> list);
        bool operator==(const vector& vec) const;
        void expand();
        void push_back(const T& value);
        void pop_back();
        void reserve(int capacity);
        void resize(int new_size, const T& value = T());
        void assign(int size, const T& value);
        template<typename inputIterator,
            typename = MyStl::enable_if_t<!MyStl::is_integral_v<inputIterator>>>
        void assign(inputIterator first, inputIterator last);
        template<typename... Args>
        void emplace_back(Args&&... args) noexcept;
        iterator erase(iterator pos);
        iterator insert(iterator pos, const T& value);
        int size() const;
        void clear();
        bool empty() const { return m_size == 0; }
        reference front() { return Vec[0]; }
        const_reference front() const { return Vec[0]; }
        reference back() { return Vec[m_size - 1]; }
        const_reference back() const { return Vec[m_size - 1]; }
    };

    template<typename T, typename Alloc>
    vector<T, Alloc>::vector() : Vec(nullptr), m_size(0), m_capacity(0) {}
    template<typename T, typename Alloc>
    vector<T, Alloc>::vector(const vector& vec): m_size(vec.m_size), m_capacity(vec.m_capacity) {
        Vec = allocator_type::allocate(m_capacity);
        for (int i = 0; i < m_size; ++i) {
            allocator_type::construct(Vec + i, vec.Vec[i]);
        }
    }

    template<typename T, typename Alloc>
    vector<T, Alloc>::vector(int size, const T& value) : Vec(nullptr), m_size(0), m_capacity(0) {
        reserve(size);
        for (int i = 0; i < size; ++i) {
            allocator_type::construct(Vec + i, value);
        }
        m_size = size;
    }

    template<typename T, typename Alloc>
    template<typename inputIterator, typename>
    vector<T, Alloc>::vector(inputIterator first, inputIterator last) : Vec(nullptr), m_size(0), m_capacity(0) {
        difference_type len = MyStl::distance(first, last);
        reserve(len);
        for (; first != last; ++first) {
            MyStl::construct(Vec + m_size, *first);
            ++m_size;
        }
    }

    template<typename T, typename Alloc>
    vector<T, Alloc>::vector(vector&& vec) : m_size(vec.m_size), m_capacity(vec.m_capacity) {
        m_size = vec.m_size;
        m_capacity = vec.m_capacity;
        Vec = vec.Vec;
        vec.Vec = nullptr;
        vec.m_size = 0;
        vec.m_capacity = 0;
    }

    template<typename T, typename Alloc>
    vector<T, Alloc>::vector(std::initializer_list<T> list) : m_size(list.size()), m_capacity(list.size()) {
        Vec = allocator_type::allocate(m_capacity);
        int index = 0;
        for (const auto& item : list) {
            allocator_type::construct(Vec + index, item);
            ++index;
        }
    }

    template<typename T, typename Alloc>
    vector<T, Alloc>::~vector() {
        clear();
        if (m_capacity > 0) 
            allocator_type::deallocate(Vec, m_capacity);
        Vec = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    template<typename T, typename Alloc>
    T& vector<T, Alloc>::operator[](int index) {
        if (index < 0 || index >= m_size) {
            throw std::out_of_range("Index out of range");
        }
        return Vec[index];
    }

    template<typename T, typename Alloc>
    typename vector<T, Alloc>::const_reference vector<T, Alloc>::operator[](int index) const {
        if (index < 0 || index >= m_size) {
            throw std::out_of_range("Index out of range");
        }
        return Vec[index];
    }

    template<typename T, typename Alloc>
    vector<T, Alloc>& vector<T, Alloc>::operator=(const vector& vec) {
        if (this == &vec) {
            return *this;
        }
        if (m_capacity < vec.m_size) {
            reserve(vec.m_size);
        }
        for (int i = 0; i < vec.m_size; ++i) {
            if (i < m_size) {
                Vec[i] = vec.Vec[i]; // 对存活对象，调用覆盖赋值 operator=
            } else {
                allocator_type::construct(Vec + i, vec.Vec[i]); // 对未初始化的内存，调用 construct
            }
        }
        for (int i = vec.m_size; i < m_size; ++i) {
            allocator_type::destroy(Vec + i);
        }
        m_size = vec.m_size;
        m_capacity = vec.m_capacity;
        return *this;
    }

    template<typename T, typename Alloc>
    vector<T, Alloc>& vector<T, Alloc>::operator=(std::initializer_list<T> list) {
        m_size = list.size();
        m_capacity = list.size();
        Vec = allocator_type::allocate(m_capacity);
        int index = 0;
        for (const auto& item : list) {
            allocator_type::construct(Vec + index, item);
            ++index;
        }
        return *this;
    }

    template<typename T, typename Alloc>
    typename vector<T, Alloc>::iterator vector<T, Alloc>::begin() {
        return iterator(Vec);
    }

    template<typename T, typename Alloc>
    typename vector<T, Alloc>::iterator vector<T, Alloc>::end() {
        return iterator(Vec + m_size);
    }

    template<typename T, typename Alloc>
    typename vector<T, Alloc>::reverse_iterator vector<T, Alloc>::rbegin() {
        return reverse_iterator(end());
    }

    template<typename T, typename Alloc>
    typename vector<T, Alloc>::reverse_iterator vector<T, Alloc>::rend() {
        return reverse_iterator(begin());
    }

    template<typename T, typename Alloc>
    typename vector<T, Alloc>::const_reverse_iterator vector<T, Alloc>::rbegin() const {
        return const_reverse_iterator(end());
    }   

    template<typename T, typename Alloc>
    typename vector<T, Alloc>::const_reverse_iterator vector<T, Alloc>::rend() const {
        return const_reverse_iterator(begin());
    }

    template<typename T, typename Alloc>
    typename vector<T, Alloc>::const_iterator vector<T, Alloc>::begin() const {
        return const_iterator(Vec);
    }

    template<typename T, typename Alloc>
    typename vector<T, Alloc>::const_iterator vector<T, Alloc>::end() const {
        return const_iterator(Vec + m_size);
    }

    template<typename T, typename Alloc>
    typename vector<T, Alloc>::const_iterator vector<T, Alloc>::cbegin() const {
        return const_iterator(Vec);
    }

    template<typename T, typename Alloc>
    typename vector<T, Alloc>::const_iterator vector<T, Alloc>::cend() const {
        return const_iterator(Vec + m_size);
    }

    template<typename T, typename Alloc>
    vector<T, Alloc>& vector<T, Alloc>::operator=(vector&& vec) noexcept {
        if (this == &vec) {
            return *this;
        }
        clear();
        allocator_type::deallocate(Vec, m_capacity);
        Vec = vec.Vec;
        m_size = vec.m_size;
        m_capacity = vec.m_capacity;
        vec.Vec = nullptr;
        vec.m_size = 0;
        vec.m_capacity = 0;
        return *this;
    }

    template<typename T, typename Alloc>
    bool vector<T, Alloc>::operator==(const vector& vec) const {
        if (this == &vec) {
            return true;
        }
        if (m_size != vec.m_size) {
            return false;
        }
        for (int i = 0; i < m_size; ++i) {
            if (Vec[i] != vec.Vec[i]) {
                return false;
            }
        }
        return true;
    }

    template<typename T, typename Alloc>
    void vector<T, Alloc>::expand() {
        int old_capacity = m_capacity;
        m_capacity = (m_capacity == 0) ? 1 : m_capacity * 2;
        
        T* newVec = allocator_type::allocate(m_capacity);
        
        // 强异常安全防线：如果搬运失败，把刚申请的新内存退回去！
        try {
            if (MyStl::is_nothrow_move_constructible_v<T>) {
                MyStl::uninitialized_move(Vec, Vec + m_size, newVec);
            } else {
                MyStl::uninitialized_copy(Vec, Vec + m_size, newVec);
            }
        } catch (...) {
            allocator_type::deallocate(newVec, m_capacity);
            m_capacity = old_capacity; // 恢复容量状态
            throw; // 继续向上抛出
        }

        // 全部成功后，再销毁旧数据、释放旧内存
        if (Vec != nullptr) {
            allocator_type::destroy(Vec, Vec + m_size);
            allocator_type::deallocate(Vec, old_capacity); // 修复内存泄漏！
        }
        
        Vec = newVec;
    }

    template<typename T, typename Alloc>
    void vector<T, Alloc>::push_back(const T& value) {
        if (m_size >= m_capacity) {
            expand();
        }
        allocator_type::construct(Vec + m_size, value);
        ++m_size;
    }

    template<typename T, typename Alloc>
    void vector<T, Alloc>::pop_back() {
        if (m_size > 0) {
            allocator_type::destroy(Vec + m_size - 1);
            --m_size;
        }
    }

    template<typename T, typename Alloc>
    void vector<T, Alloc>::reserve(int capacity) {
        if (capacity < m_capacity) return; // 只有当新容量大于当前容量时才进行反转和扩容
        T* newVec = allocator_type::allocate(capacity); // 扩容三步走 申请 -> 搬家 -> 释放旧空间
        try {
            if (MyStl::is_nothrow_move_constructible_v<T>) {
                MyStl::uninitialized_move(Vec, Vec + m_size, newVec);
            } else {
                MyStl::uninitialized_copy(Vec, Vec + m_size, newVec);
            }
        } catch (...) {
            allocator_type::deallocate(newVec, capacity);
            throw;
        }
        if (Vec != nullptr) {
            allocator_type::destroy(Vec, Vec + m_size);
            allocator_type::deallocate(Vec, m_capacity);
        }
        Vec = newVec;
        m_capacity = capacity;
    }

    template<typename T, typename Alloc>
    void vector<T, Alloc>::resize(int new_size, const T& value) {
        if (new_size < m_size) {
            // 缩小：析构多余的
            for (int i = new_size; i < m_size; ++i) {
                allocator_type::destroy(Vec + i);
            }
        } else if (new_size > m_size) {
            // 增大：如果超过容量，先扩容
            if (new_size > m_capacity) {
                // 策略：要么翻倍，要么直接够用，取大者
                int new_cap = (m_capacity * 2 > new_size) ? m_capacity * 2 : new_size;
                reserve(new_cap);
            }
            // 构造新增的元素
            for (int i = m_size; i < new_size; ++i) {
                allocator_type::construct(Vec + i, value);
            }
        }
        m_size = new_size;
    }

    template<typename T, typename Alloc>
    void vector<T, Alloc>::assign(int size, const T& value) {
        if (size > m_capacity) {
            reserve(size);
        }
        for (int i = 0; i < size; ++i) {
            if (i < m_size) {
                Vec[i] = value; // 已经存在的元素直接赋值
            } else {
                allocator_type::construct(Vec + i, value); // 新增元素需要构造
            }
        }
        // 如果新 size 小于当前 size，析构多余的元素
        for (int i = size; i < m_size; ++i) {
            allocator_type::destroy(Vec + i);
        }
        m_size = size;
    }

    template<typename T, typename Alloc>
    template<typename inputIterator, typename>
    void vector<T, Alloc>::assign(inputIterator first, inputIterator last) {
        difference_type len = MyStl::distance(first, last);
        if (len > m_capacity) {
            reserve(len);
        }
        for (int i = 0; i < len; ++i) {
            if (i < m_size) {
                Vec[i] = *first;
                ++first;
            } else {
                allocator_type::construct(Vec + i, *first);
                ++first;
            }
        }
        for (int i = len; i < m_size; ++i) {
            allocator_type::destroy(Vec + i);
        }
        m_size = len;
    }

    template<typename T, typename Alloc>
    typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator pos) {
        if (pos < begin() || pos >= end()) {
            throw std::out_of_range("Iterator out of range");
        }
        for (iterator i = pos; i + 1 < end(); ++i) {
            *i = MyStl::move(*(i + 1));
        }
        allocator_type::destroy(Vec + m_size - 1);
        --m_size;
        return pos;
    }

    template<typename T, typename Alloc>
    typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator pos, const T& value) {
        int index = pos - begin();
        if (index < 0 || index > m_size) {
            throw std::out_of_range("Iterator out of range");
        }

        T old_value = value; // 先保存一份 value，防止后续移动时被覆盖

        if (m_size >= m_capacity) {
            int new_capacity = (m_capacity == 0) ? 1 : m_capacity * 2;
            T* newVec = allocator_type::allocate(new_capacity);

            for (int i = 0; i < index; ++i) {
                allocator_type::construct(newVec + i, MyStl::move(Vec[i]));
                allocator_type::destroy(Vec + i);
            }
            allocator_type::construct(newVec + index, old_value);
            for (int i = index; i < m_size; ++i) {
                allocator_type::construct(newVec + i + 1, MyStl::move(Vec[i]));
                allocator_type::destroy(Vec + i);
            }

            if (m_capacity > 0) {
                allocator_type::deallocate(Vec, m_capacity);
            }
            Vec = newVec;
            m_capacity = new_capacity;
        }
        else {
            if (index == m_size) {
                allocator_type::construct(Vec + m_size, old_value);
            } else {
                allocator_type::construct(Vec + m_size, MyStl::move(Vec[m_size - 1]));
                for (int i = m_size - 1; i > index; --i) {
                    Vec[i] = MyStl::move(Vec[i - 1]);
                }
                Vec[index] = old_value;
            }
        }
        m_size++;
        return begin() + index;
    }

    template<typename T, typename Alloc>
    template<typename... Args>
    void vector<T, Alloc>::emplace_back(Args&&... args) noexcept {
        if (m_size >= m_capacity) {
            expand();
        }
        allocator_type::construct(Vec + m_size, MyStl::forward<Args>(args)...);
        ++m_size;
    }

    template<typename T, typename Alloc>
    int vector<T, Alloc>::size() const {
        return this->m_size;
    }

    template<typename T, typename Alloc>
    void vector<T, Alloc>::clear() {
        for (int i = 0; i < m_size; ++i) {
            allocator_type::destroy(Vec + i);
        }
        m_size = 0;
    }
}