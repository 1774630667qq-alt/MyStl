#pragma once
#include <cstddef>
#include "iterator.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "allocator.hpp"

namespace MyStl {

    // 1. 节点定义 (双向链表)
    template<typename T>
    struct ListNode {
        T data;
        ListNode* prev;
        ListNode* next;

        // 构造函数
        ListNode(const T& val = T()) : data(val), prev(nullptr), next(nullptr) {}
    };

    // 2. 迭代器定义 (补充了 Traits)
    template<typename T, bool IsConst = false>
    struct ListIterator {
        // =========================================================
        // 🌟 迭代器的 5 个标准"身份证信息" (替代继承)
        using iterator_category = MyStl::bidirectional_iterator_tag; // 迭代器类别
        using value_type        = T;                               // 元素类型
        using difference_type   = ptrdiff_t;                       // 距离类型
        using pointer           = typename MyStl::conditional_t<IsConst, const T*, T*>;
        using reference         = typename MyStl::conditional_t<IsConst, const T&, T&>;
        // =========================================================

        using ptr_type = typename MyStl::conditional_t<IsConst, const T*, T*>;

        using Node = ListNode<T>;
        Node* ptr; 

        ListIterator(Node* p = nullptr) : ptr(p) {}

        // TODO 1: 解引用运算符
        reference operator*() const {
            return ptr->data;
        }

        // TODO 2: 箭头运算符
        pointer operator->() const {
            return &ptr->data;
        }

        // TODO 3: 前置 ++ (++it)
        ListIterator& operator++() {
            ptr = ptr->next;
            return *this;
        }

        // TODO 4: 后置 ++ (it++)
        ListIterator operator++(int) {
            ListIterator temp = *this; 
            ptr = ptr->next;           
            return temp;               
        }

        ListIterator& operator--() {
            ptr = ptr->prev;
            return *this;
        }

        ListIterator operator--(int) {
            ListIterator temp = *this;
            ptr = ptr->prev;
            return temp;
        }

        // TODO 5: 判等
        bool operator==(const ListIterator& other) const {
            return ptr == other.ptr;
        }
        bool operator!=(const ListIterator& other) const {
            return ptr != other.ptr;
        }
    };

    // 3. 链表主体类 (预览，先不用实现具体逻辑)
    template<typename T, typename Alloc = MyStl::allocator<T>>
    class list {
    public:
        using iterator = ListIterator<T>;
        using const_iterator = ListIterator<T, true>;
        using pointer = T*;
        using reference = T&;
        using const_pointer = const T*;
        using const_reference = const T&;
        using Node = ListNode<T>;
        using node_allocator = typename Alloc::template rebind<Node>::other;

    private:
        // 思考：为了方便，我们要不要在这里用一个 dummy_node (哨兵节点)？
        /*
        由于当前类为双端队列所以链表尾端应该和链表首端相连，如果添加一个哨兵节点反而人为的将首端节点和尾端节点隔绝开来了
        结论：不需要，直接直接将首端节点视为尾端节点的后继，尾端节点视为首端节点的前驱即可
        */
        Node* Node_; // 哨兵节点 
        size_t size_;

    public:
        list();
        list(const list& other);
        list(list&& other) noexcept;
        list& operator=(const list& other);
        list& operator=(list&& other) noexcept;
        ~list();
        void push_back(const T& val);
        void push_front(const T& val);
        void pop_back();
        void pop_front();
        bool empty() const;
        void clean();
        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;
        size_t size() const;
        iterator insert(iterator pos, const T& val);
        iterator erase(iterator pos);
        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;
    };

    template<typename T, typename Alloc>
    list<T, Alloc>::list(): size_(0) {
        Node_ = node_allocator::allocate(1);
        node_allocator::construct(Node_); // 构造哨兵节点
        Node_->prev = Node_;
        Node_->next = Node_;
    }

    template<typename T, typename Alloc>
    list<T, Alloc>::list(const list& other) : size_(0) {
        Node_ = node_allocator::allocate(1);
        node_allocator::construct(Node_);
        Node_->prev = Node_;
        Node_->next = Node_;
        for (const auto& val : other) {
            push_back(val);
        }
    }

    template<typename T, typename Alloc>
    list<T, Alloc>::list(list&& other) noexcept : size_(0) {
        Node_ = node_allocator::allocate(1);
        node_allocator::construct(Node_);
        Node_->prev = Node_;
        Node_->next = Node_;
        MyStl::swap(size_, other.size_);
        MyStl::swap(Node_, other.Node_);
    }

    template<typename T, typename Alloc>
    list<T, Alloc>& list<T, Alloc>::operator=(const list& other) {
        if (this != &other) {
            clean();
            for (const auto& val : other) {
                push_back(val);
            }
        }
        return *this;
    }

    template<typename T, typename Alloc>
    list<T, Alloc>& list<T, Alloc>::operator=(list&& other) noexcept {
        if (this != &other) {
            MyStl::swap(size_, other.size_);
            MyStl::swap(Node_, other.Node_);
        }
        return *this;
    }

    template<typename T, typename Alloc>
    typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator pos, const T& val) {
        Node* p = pos.ptr;
        Node* newnode = node_allocator::allocate(1);
        node_allocator::construct(newnode, val);
        newnode->prev = p->prev;
        newnode->next = p;
        p->prev->next = newnode;
        p->prev = newnode;
        ++size_;
        return iterator(newnode);
    }

    template<typename T, typename Alloc>
    void list<T, Alloc>::clean() {
        while (size_ > 0) {
            pop_back();
        }
    }

    template<typename T, typename Alloc>
    size_t list<T, Alloc>::size() const {
        return size_;
    }

    template<typename T, typename Alloc>
    typename list<T, Alloc>::reference list<T, Alloc>::front() {
        return Node_->next->data;
    }

    template<typename T, typename Alloc>
    typename list<T, Alloc>::const_reference list<T, Alloc>::front() const {
        return Node_->next->data;
    }

    template<typename T, typename Alloc>
    typename list<T, Alloc>::reference list<T, Alloc>::back() {
        return Node_->prev->data;
    }

    template<typename T, typename Alloc>
    typename list<T, Alloc>::const_reference list<T, Alloc>::back() const {
        return Node_->prev->data;
    }

    template<typename T, typename Alloc>
    typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator pos) {
        if (pos == end()) return pos; // 如果是 end，直接返回 end
        Node* p = pos.ptr;
        Node* next_node = p->next;
        p->prev->next = p->next;
        p->next->prev = p->prev;
        node_allocator::destroy(p);
        node_allocator::deallocate(p, 1);
        --size_;
        return iterator(next_node); // 返回下一个节点的迭代器
    }

    template<typename T, typename Alloc>
    void list<T, Alloc>::pop_back() {
        if (size_ > 0) {
            erase(Node_->prev);
        }
    }

    template<typename T, typename Alloc>
    void list<T, Alloc>::pop_front() {
        if (size_ > 0) {
            erase(Node_->next);
        }
    }

    template<typename T, typename Alloc>
    bool list<T, Alloc>::empty() const {
        return size_ == 0;
    }

    template<typename T, typename Alloc>
    void list<T, Alloc>::push_back(const T& val) {
        insert(Node_, val);
    }

    template<typename T, typename Alloc>
    void list<T, Alloc>::push_front(const T& val) {
        insert(begin(), val);
    }

    template<typename T, typename Alloc>
    typename list<T, Alloc>::iterator list<T, Alloc>::begin() {
        return iterator(Node_->next);
    }

    template<typename T, typename Alloc>
    typename list<T, Alloc>::iterator list<T, Alloc>::end() {
        return iterator(Node_);
    }

    template<typename T, typename Alloc>
    typename list<T, Alloc>::const_iterator list<T, Alloc>::begin()const {
        return const_iterator(Node_->next);
    }

    template<typename T, typename Alloc>
    typename list<T, Alloc>::const_iterator list<T, Alloc>::end() const {
        return const_iterator(Node_);
    }

    template<typename T, typename Alloc>
    list<T, Alloc>::~list() {
        clean();
        node_allocator::destroy(Node_);
        node_allocator::deallocate(Node_, 1);
    }
}