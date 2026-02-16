#pragma once
#include <cstddef>
#include <stdexcept>
#include "unique_ptr.h"
#include <initializer_list>


namespace MyStl {
    template<typename Vector>
    class VectorIterator
    {
        public:
        using PointValue = typename Vector::ValueType;
        using Pointer = PointValue*;
        using Reference = PointValue&;
        private:
        Pointer ptr;
        public:
        VectorIterator(Pointer ptr) : ptr(ptr) {}

        Reference operator*() const {
            return *ptr;
        }

        Reference operator[](int index) const {
            return *(ptr + index);
        }

        Pointer operator++() {
            ++ptr;
            return ptr;
        }

        Pointer operator++(int) {
            Pointer temp = ptr;
            ptr++;
            return temp;
        }

        Pointer operator--() {
            --ptr;
            return ptr;
        }

        Pointer operator--(int) {
            Pointer temp = ptr;
            ptr--;
            return temp;
        }

        VectorIterator& operator+=(int offset) {
            ptr += offset;
            return *this;
        } 

        VectorIterator operator+(int offset) const {
            return VectorIterator(ptr + offset);
        }

        VectorIterator& operator-=(int offset) {
            ptr -= offset;
            return *this;
        }

        VectorIterator operator-(int offset) const {
            return VectorIterator(ptr - offset);
        }

        std::ptrdiff_t operator-(const VectorIterator& other) const {
            return ptr - other.ptr;
        }

        bool operator <(const VectorIterator& other) const {
            return ptr < other.ptr;
        }

        bool operator <=(const VectorIterator& other) const {
            return ptr <= other.ptr;
        }

        bool operator >(const VectorIterator& other) const {
            return ptr > other.ptr;
        }

        bool operator >=(const VectorIterator& other) const {
            return ptr >= other.ptr;
        }

        bool operator==(const VectorIterator& other) const {
            return ptr == other.ptr;
        }

        bool operator!=(const VectorIterator& other) const {
            return ptr != other.ptr;
        }
    };

    template<typename T>
    class Vector
    {
        public:
        using ValueType = T;
        using Iterator = VectorIterator<Vector<T>>;
        private:
        MyStl::unique_ptr<T[]> Vec;
        int m_size;
        int m_capacity;
        public:
        Vector();
        Vector(int capacity);
        Vector(std::initializer_list<T> list);
        Vector(const Vector& vec);
        Vector(Vector&& vec);
        ~Vector();
        T& operator[](int index);
        Iterator begin() const;
        Iterator end() const;
        Vector& operator=(const Vector& vec);
        Vector& operator=(Vector&& vec) noexcept;
        Vector& operator=(std::initializer_list<T> list);
        bool operator==(const Vector& vec) const;
        void expand();
        void push_back(const T& value);
        void pop_back();
        void emplace_back(T&& value) noexcept;
        Iterator erase(Iterator pos);
        Iterator insert(Iterator pos, const T& value);
        int size() const;
        void clear();
    };

    template<typename T>
    Vector<T>::Vector() : Vec(nullptr), m_size(0), m_capacity(0) {}
    template<typename T>
    Vector<T>::Vector(const Vector<T>& vec): m_size(vec.m_size), m_capacity(vec.m_capacity) {
        Vec = MyStl::make_unique<T[]>(m_capacity);
        for (int i = 0; i < m_size; ++i) {
            Vec[i] = vec.Vec[i];
        }
    }

    template<typename T>
    Vector<T>::Vector(int capacity) : Vec(MyStl::make_unique<T[]>(capacity)), m_size(0), m_capacity(capacity) {}
    template<typename T>
    Vector<T>::Vector(Vector<T>&& vec) : Vec(std::move(vec.Vec)), m_size(vec.m_size), m_capacity(vec.m_capacity) {
        vec.Vec = nullptr;
        vec.m_size = 0;
        vec.m_capacity = 0;
    }

    template<typename T>
    Vector<T>::Vector(std::initializer_list<T> list) : 
        Vec(MyStl::make_unique<T[]>(list.size())), m_size(list.size()), m_capacity(list.size()) {
        int index = 0;
        for (const auto& item : list) {
            Vec[index++] = item;
        }
    }

    template<typename T>
    Vector<T>::~Vector() {
        m_size = 0;
        m_capacity = 0;
    }

    template<typename T>
    T& Vector<T>::operator[](int index) {
        if (index < 0 || index >= m_size) {
            throw std::out_of_range("Index out of range");
        }
        return Vec[index];
    }

    template<typename T>
    Vector<T>& Vector<T>::operator=(const Vector<T>& vec) {
        if (this == &vec) {
            return *this;
        }
        m_size = vec.m_size;
        m_capacity = vec.m_capacity;
        Vec = MyStl::make_unique<T[]>(m_capacity);
        for (int i = 0; i < m_size; ++i) {
            Vec[i] = vec.Vec[i];
        }
        return *this;
    }

    template<typename T>
    Vector<T>& Vector<T>::operator=(std::initializer_list<T> list) {
        m_size = list.size();
        m_capacity = list.size();
        Vec = MyStl::make_unique<T[]>(m_capacity);
        int index = 0;
        for (const auto& item : list) {
            Vec[index++] = item;
        }
        return *this;
    }

    template<typename T>
    typename Vector<T>::Iterator Vector<T>::begin() const {
        return Iterator(Vec.get());
    }

    template<typename T>
    typename Vector<T>::Iterator Vector<T>::end() const {
        return Iterator(Vec.get() + m_size);
    }

    template<typename T>
    Vector<T>& Vector<T>::operator=(Vector<T>&& vec) noexcept {
        if (this == &vec) {
            return *this;
        }
        Vec = std::move(vec.Vec);
        m_size = vec.m_size;
        m_capacity = vec.m_capacity;
        vec.Vec = nullptr;
        vec.m_size = 0;
        vec.m_capacity = 0;
        return *this;
    }

    template<typename T>
    bool Vector<T>::operator==(const Vector<T>& vec) const {
        if (this == &vec) {
            return true;
        }
        return false;
    }

    template<typename T>
    void Vector<T>::expand() {
        if (m_capacity == 0) {
            m_capacity = 1;
        }
        m_capacity *= 2;
        MyStl::unique_ptr<T[]> newVec = MyStl::make_unique<T[]>(m_capacity);
        for (int i = 0; i < m_size; i++) {
            newVec[i] = Vec[i];
        }
        Vec = std::move(newVec);
        newVec = nullptr;
    }

    template<typename T>
    void Vector<T>::push_back(const T& value) {
        if (m_size >= m_capacity) {
            expand();
        }
        Vec[m_size++] = value;
    }

    template<typename T>
    void Vector<T>::pop_back() {
        if (m_size > 0) {
            --m_size;
        }
    }

    template<typename T>
    typename Vector<T>::Iterator Vector<T>::erase(Iterator pos) {
        if (pos < begin() || pos >= end()) {
            throw std::out_of_range("Iterator out of range");
        }
        for (Iterator i = pos; i + 1 < end(); ++i) {
            *i = std::move(*(i + 1));
        }
        --m_size;
        return pos;
    }

    template<typename T>
    typename Vector<T>::Iterator Vector<T>::insert(Iterator pos, const T& value) {
        int index = pos - begin();
        if (m_size >= m_capacity) {
            expand();
        }
        for (int i = m_size; i > index; --i) {
            Vec[i] = std::move(Vec[i - 1]);
        }
        Vec[index] = value;
        m_size++;
        return begin() + index;
    }

    template<typename T>
    void Vector<T>::emplace_back(T&& value) noexcept {
        if (m_size >= m_capacity) {
            expand();
        }
        Vec[m_size++] = std::move(value);
    }

    template<typename T>
    int Vector<T>::size() const {
        return this->m_size;
    }

    template<typename T>
    void Vector<T>::clear() {
        Vec = nullptr;
        m_size = 0;
        m_capacity = 0;
    }
}