/*
 * @Author: Zhang YuHua 1774630667@qq.com
 * @Date: 2026-03-14 14:11:31
 * @LastEditors: Zhang YuHua 1774630667@qq.com
 * @LastEditTime: 2026-03-14 15:39:08
 * @FilePath: /MyStl/include/forward_list.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include <cstddef>
#include "iterator.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "allocator.hpp"

namespace MyStl {
    template<typename T>
    struct ForwardListNodeBase {
        ForwardListNodeBase* next;
        ForwardListNodeBase() : next(nullptr) {}
    };

    template<typename T>
    struct ForwardListNode : public ForwardListNodeBase<T>{
        T data;

        ForwardListNode(const T& val = T()) : data(val) {}
    };

    template<typename T, typename Alloc = MyStl::allocator<T>>
    class ForwardList;

    template<typename T, bool IsConst = false>
    class ForwardListIterator {
        friend class ForwardList<T>;
        
        public:
        using iterator_category = MyStl::forward_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = MyStl::conditional_t<IsConst, const T*, T*>;
        using reference = MyStl::conditional_t<IsConst, const T&, T&>;
        using const_reference = const T&;
        using const_pointer = const T*;
        using Node = ForwardListNodeBase<T>;

        private:
        Node* ptr;

        public:
        ForwardListIterator(Node* p = nullptr) : ptr(p) {}
        reference operator*() const { return static_cast<ForwardListNode<T>*>(ptr)->data; }
        pointer operator->() const { return &(operator*()); }
        ForwardListIterator& operator++() { ptr = ptr->next; return *this; }
        ForwardListIterator operator++(int) { ForwardListIterator temp = *this; ptr = ptr->next; return temp; }
        bool operator==(const ForwardListIterator& other) const { return ptr == other.ptr; }
        bool operator!=(const ForwardListIterator& other) const { return ptr != other.ptr; }
    };

    template<typename T, typename Alloc = MyStl::allocator<T>>
    class ForwardList {
        public:
        using iterator = ForwardListIterator<T>;
        using const_iterator = ForwardListIterator<T, true>;
        using pointer = T*;
        using reference = T&;
        using const_pointer = const T*;
        using const_reference = const T&;
        using Node = ForwardListNodeBase<T>;
        using ForwardListNode = ForwardListNode<T>;
        using node_allocator = Alloc::template rebind<ForwardListNode>::other;

        private:
        Node Node_;
        public:
        ForwardList();
        ForwardList(const ForwardList& other);
        ForwardList(ForwardList&& other) noexcept;
        ForwardList& operator=(const ForwardList& other);
        ForwardList& operator=(ForwardList&& other) noexcept;
        void push_front(const T& val);
        void pop_front();
        bool empty() const;
        void clean();
        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;
        iterator before_begin();
        iterator insert_after(iterator pos, const T& val);
        iterator erase_after(iterator pos);
        ~ForwardList();
    };

    template<typename T, typename Alloc>
    ForwardList<T, Alloc>::ForwardList() {
        Node_.next = nullptr;
    }

    template<typename T, typename Alloc>
    ForwardList<T, Alloc>::ForwardList(const ForwardList& other) {
        Node_.next = nullptr;
        iterator curr = before_begin(); // 修复为 iterator
        for (const auto& val : other) {
            curr = insert_after(curr, val);
        }
    }
    
    template<typename T, typename Alloc>
    ForwardList<T, Alloc>::ForwardList(ForwardList&& other) noexcept {
        Node_.next = nullptr;
        MyStl::swap(Node_.next, other.Node_.next);
    }

    template<typename T, typename Alloc>
    ForwardList<T, Alloc>& ForwardList<T, Alloc>::operator=(const ForwardList& other) {
        if (this != &other) {
            clean();
            iterator curr = before_begin();
            for (const auto& val : other) {
                curr = insert_after(curr, val); // 修复原逻辑会导致链表元素倒序的问题
            }
        }
        return *this;
    }

    template<typename T, typename Alloc>
    ForwardList<T, Alloc>& ForwardList<T, Alloc>::operator=(ForwardList&& other) noexcept {
        if (this != &other) {
            clean();
            MyStl::swap(Node_.next, other.Node_.next); // 顺手补全 MyStl:: 防止 ADL 匹配异常
        }
        return *this;
    }

    template<typename T, typename Alloc>
    typename ForwardList<T, Alloc>::iterator ForwardList<T, Alloc>::before_begin() {
        return iterator(&Node_);
    }

    template<typename T, typename Alloc>
    typename ForwardList<T, Alloc>::iterator ForwardList<T, Alloc>::begin() {
        return iterator(Node_.next);
    }

    template<typename T, typename Alloc>
    typename ForwardList<T, Alloc>::iterator ForwardList<T, Alloc>::end() {
        return iterator(nullptr);
    }

    template<typename T, typename Alloc> 
    typename ForwardList<T, Alloc>::const_iterator ForwardList<T, Alloc>::begin() const {
        return const_iterator(Node_.next);
    }

    template<typename T, typename Alloc>
    typename ForwardList<T, Alloc>::const_iterator ForwardList<T, Alloc>::end() const {
        return const_iterator(nullptr);
    }

    template<typename T, typename Alloc>
    void ForwardList<T, Alloc>::push_front(const T& val) {
        Node* newnode = node_allocator::allocate(1);
        node_allocator::construct(newnode, val);
        newnode->next = Node_.next;
        Node_.next = newnode;
    }

    template<typename T, typename Alloc>
    void ForwardList<T, Alloc>::pop_front() {
        if (!empty()) {
            Node* p = static_cast<ForwardListNode<T>* >(Node_.next);
            Node_.next = p->next;
            node_allocator::destroy(p);
            node_allocator::deallocate(p, 1);
        }
    }

    template<typename T, typename Alloc>
    bool ForwardList<T, Alloc>::empty() const {
        return Node_.next == nullptr;
    }

    template<typename T, typename Alloc>
    void ForwardList<T, Alloc>::clean() {
        while (!empty()) {
            pop_front();
        }
    }

    template<typename T, typename Alloc>
    typename ForwardList<T, Alloc>::iterator ForwardList<T, Alloc>::insert_after(iterator pos, const T& val) {
        Node* p = pos.ptr;
        Node* newnode = node_allocator::allocate(1);
        node_allocator::construct(newnode, val);
        newnode->next = p->next;
        p->next = newnode;
        return iterator(newnode);
    }

    template<typename T, typename Alloc>
    typename ForwardList<T, Alloc>::iterator ForwardList<T, Alloc>::erase_after(iterator pos) {
        if (pos == end()) return pos;
        Node* p = pos.ptr;
        Node* next_node = static_cast<ForwardListNode<T>* >(p->next);
        p->next = next_node->next;
        node_allocator::destroy(next_node);
        node_allocator::deallocate(next_node, 1);
        return iterator(p->next);
    }

    template<typename T, typename Alloc>
    ForwardList<T, Alloc>::~ForwardList() {
        clean();
    }    
}