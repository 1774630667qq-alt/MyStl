/*
 * @Author: Zhang YuHua 1774630667@qq.com
 * @Date: 2026-03-14 16:00:31
 * @LastEditors: Zhang YuHua 1774630667@qq.com
 * @LastEditTime: 2026-03-14 17:41:11
 * @FilePath: /MyStl/include/hashtable.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include "utility.hpp"
#include <cstddef>
#include "vector.hpp"
#include "functional.hpp"
#include "allocator.hpp"

namespace MyStl {
    struct HashTableBase {
        HashTableBase* next;
        HashTableBase() : next(nullptr) {}
    };

    template<typename T>
    struct HashTableNode : public HashTableBase {
        T data;
        HashTableNode(const T& d) : data(d) {}
    };

    template<typename T, bool IsConst = false>
    class HashTableIterator {
        friend class HashTableIterator<T, !IsConst>;

        public:
        using iterator_category = MyStl::forward_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = MyStl::conditional_t<IsConst, const T*, T*>;
        using reference         = MyStl::conditional_t<IsConst, const T&, T&>;
        using Node              = HashTableNode<T>; // 迭代器持有子类节点
        using link_type         = Node;

        private:
        link_type* ptr;

        public:
        HashTableIterator(link_type* p = nullptr) : ptr(p) {}
        HashTableIterator(const HashTableIterator<T, !IsConst>& other) : ptr(other.ptr) {}

        reference operator*() const { return static_cast<HashTableNode<T>*>(ptr)->data; }
        pointer operator->() const { return &static_cast<HashTableNode<T>*>(ptr)->data; }

        HashTableIterator& operator++() {
            ptr = static_cast<link_type*>(ptr->next);
            return *this;
        }
        HashTableIterator operator++(int) {
            HashTableIterator temp = *this;
            ptr = static_cast<link_type*>(ptr->next);
            return temp;
        }
        bool operator==(const HashTableIterator& other) const { return ptr == other.ptr; }
        bool operator!=(const HashTableIterator& other) const { return ptr != other.ptr; }
    };

    template <typename Value,      // 存入容器的真实数据类型（Set是Key，Map是Pair）
          typename Key,        // 用于计算哈希的键类型
          typename HashFcn,    // 哈希函数 (如 MyStl::hash)
          typename ExtractKey, // 从 Value 中提取 Key 的仿函数 (如 select1st 或 identity)
          typename EqualKey = MyStl::equal_to<Key>,   // 判断两个 Key 是否相等的仿函数 (如 MyStl::equal_to)
          typename Alloc = MyStl::allocator<Value>>
    class hashtable { 
        public:
        using iterator = HashTableIterator<Value>;
        using const_iterator = HashTableIterator<Value, true>;
        using value_type = Value;
        using key_type = Key;

        private:
        using Node = HashTableNode<Value>;
        using node_allocator = Alloc::template rebind<Node>::other;
        using link_type = HashTableBase;

        HashFcn hasher_;
        vector<link_type*> buckets_;
        link_type Node_;
        ExtractKey key_get;
        EqualKey equal_;
        size_t size_;

        size_t bucket_index(const Key& key) const {
            return bucket_index(key);
        }

        public:
        hashtable() : buckets_(8, nullptr), size_(0) {
            Node_.next = nullptr;
        }

        iterator begin() {
            return iterator(static_cast<HashTableNode<Value>*>(Node_.next));
        }

        const_iterator begin() const {
            return const_iterator(static_cast<HashTableNode<Value>*>(Node_.next));
        }

        iterator end() {
            return iterator(nullptr);
        }

        const_iterator end() const {
            return const_iterator(nullptr);
        }

        size_t size() const {
            return size_;
        }

        bool empty() const {
            return size_ == 0;
        }

        void clear() {
            link_type* curr = Node_.next;
            while (curr != nullptr) {
                link_type* next = curr->next;
                node_allocator::destroy(static_cast<Node*>(curr));
                node_allocator::deallocate(static_cast<Node*>(curr), 1);
                curr = next;
            }
            Node_.next = nullptr;
            buckets_.assign(buckets_.size(), nullptr);
            size_ = 0;
        }

        void rehash(size_t new_bucket_count) {
            if (new_bucket_count <= buckets_.size()) return;

            vector<link_type*> new_buckets(new_bucket_count, nullptr);
            link_type* curr = Node_.next;
            Node_.next = nullptr;
            buckets_ = MyStl::move(new_buckets);

            while (curr != nullptr) {
                link_type* next = curr->next;
                key_type key = key_get(static_cast<Node*>(curr)->data);
                size_t index = bucket_index(key);

                if (buckets_[index] == nullptr) {
                    if (Node_.next != nullptr) {
                        key_type temp_key = key_get(static_cast<Node*>(Node_.next)->data);
                        size_t old_index = hasher_(temp_key) % buckets_.size();
                        buckets_[old_index] = curr;
                    }
                    curr->next = Node_.next;
                    Node_.next = curr;
                    buckets_[index] = &Node_;
                } else {
                    curr->next = buckets_[index]->next;
                    buckets_[index]->next = curr;   
                }
                
                curr = next;
            }
        }

        iterator find(const Key& key) {
            size_t index = bucket_index(key);

            if (buckets_[index] == nullptr) {
                return end();
            }

            link_type* first_base = static_cast<link_type*>(buckets_[index]->next);
            Node* first_node = static_cast<Node*>(first_base);

            for (iterator it(first_node); it != end(); ++it) {
                if (bucket_index(key_get(*it)) != index) { // 越界检查
                    break;
                }
                if (equal_(key_get(*it), key)) {
                    return it;
                }
            }
            return end();
        }

        MyStl::pair<iterator, bool> insert_unique(const Value& value) {
            if (size_ >= buckets_.size()) {
                rehash(buckets_.size() * 2);
            }

            size_t index = bucket_index(key_get(value));
            Node* newNode = node_allocator::allocate(1);
            node_allocator::construct(newNode, value);

            if (buckets_[index] == nullptr) {
                if (Node_.next != nullptr) {
                    Node* old_first = static_cast<Node*>(Node_.next);
                    size_t old_index = bucket_index(key_get(old_first->data));
                    buckets_[old_index] = newNode;
                }

                newNode->next = Node_.next;
                Node_.next = newNode;
                buckets_[index] = &Node_;
                ++size_;
            } else {
                for (iterator it(static_cast<Node*>(buckets_[index]->next)); it != end(); ++it) {
                    if (bucket_index(key_get(*it)) != index) {
                        break;
                    }
                    if (equal_(key_get(*it), key_get(value))) {
                        // 冲突释放申请的空间
                        node_allocator::destroy(newNode);
                        node_allocator::deallocate(newNode, 1);
                        return MyStl::make_pair(it, false);
                    }
                }
                newNode->next = buckets_[index]->next;
                buckets_[index]->next = newNode;
                ++size_;
            }
            return MyStl::make_pair(iterator(newNode), true);
        }

        iterator insert_equal(const Value& value) {
            if (size_ >= buckets_.size()) {
                rehash(buckets_.size() * 2);
            }

            size_t index = bucket_index(key_get(value));
            Node* newNode = node_allocator::allocate(1);
            node_allocator::construct(newNode, value);

            if (buckets_[index] == nullptr) {
                if (Node_.next != nullptr) {
                    Node* old_first = static_cast<Node*>(Node_.next);
                    size_t old_index = bucket_index(key_get(old_first->data));
                    buckets_[old_index] = newNode;
                }

                newNode->next = Node_.next;
                Node_.next = newNode;
                buckets_[index] = &Node_;
                ++size_;
            } else {
                for (iterator it = static_cast<Node*>(buckets_[index]->next); it != end(); ++it) {
                    if (bucket_index(key_get(*it)) != index) {
                        break;
                    }
                    if (equal_(key_get(*it), key_get(value))) {
                        // 相同的key后插入
                        newNode->next = it->next;
                        it->next = newNode;
                        ++size_;
                        return iterator(newNode);
                    }
                }

                newNode->next = buckets_[index]->next;
                buckets_[index]->next = newNode;
                ++size_;
            }
            return iterator(newNode);
        }

        MyStl::pair<iterator, iterator> equal_range(const Key& key) {
            iterator it = find(key);
            if (it == end()) {
                return MyStl::make_pair(end(), end());
            }
            for (iterator curr = it; curr != end(); ++curr) {
                if (!equal_(key_get(curr->first), key)) {
                    return MyStl::make_pair(it, curr);
                }
            }
            return MyStl::make_pair(it, end());
        }

        iterator erase(const_iterator pos) { 
            if (pos == end()) {
                return end();
            }
            Node* node = static_cast<Node*>(pos.ptr);
            size_t index = bucket_index(key_get(node->data));
            link_type* prev = buckets_[index];
            while (prev->next != node) {
                prev = static_cast<link_type*>(prev->next);
                if (prev == nullptr) {
                    return end();
                }
            }
            prev->next = node->next;
            --size_;
            node_allocator::destroy(node);
            node_allocator::deallocate(node, 1);
            return iterator(prev->next);
        }

        ~hashtable() {
            clear();
        }
    };
}