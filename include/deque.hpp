#include <cstddef>
#include "type_traits.hpp"
#include "iterator.hpp"
#include "allocator.hpp"
namespace MyStl {
    inline size_t deque_buffer_size() {
        return 64; // 每个缓冲区存放64个元素
    }

    template<typename T> class deque;

    template<typename T, bool IsConst = false>
    class dequeIterator {
        friend class deque<T>;

        public:
        using iterator_category = MyStl::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = MyStl::conditional_t<IsConst, const T*, T*>;
        using reference = MyStl::conditional_t<IsConst, const T&, T&>;
        using buffer_pointer = pointer*; // 指向缓冲区指针的指针

        private:
        pointer current; // 当前元素的指针
        pointer buffer_start; // 当前缓冲区的起始地址
        pointer buffer_end; // 当前缓冲区的结束地址
        buffer_pointer node; // 指向缓冲区指针的指针

        void set_node(buffer_pointer new_node) {
            node = new_node;
            buffer_start = *node;
            buffer_end = buffer_start + deque_buffer_size();
        }

        public:
        dequeIterator() : current(nullptr), buffer_start(nullptr), buffer_end(nullptr), node(nullptr) {}

        dequeIterator(buffer_pointer n, pointer cur) :current(cur) {
            set_node(n);
        }

        reference operator*() const { return *current; }
        pointer operator->() const { return current; }
        dequeIterator& operator++() {
            ++current;
            if (current == buffer_end) { // 到达当前缓冲区末尾，切换到下一个缓冲区
                set_node(node + 1);
                current = buffer_start;
            }
            return *this;
        }
        dequeIterator operator++(int) {
            dequeIterator temp = *this;
            ++(*this);
            return temp;
        }
        dequeIterator& operator--() {
            if (current == buffer_start) { // 到达当前缓冲区起始，切换到上一个缓冲区
                set_node(node - 1);
                current = buffer_end;
            }
            --current;
            return *this;
        }
        
        dequeIterator operator--(int) {
            dequeIterator temp = *this;
            --(*this);
            return temp;
        }
        dequeIterator operator+(difference_type n) const {
            dequeIterator temp = *this;
            if (n >= 0) {
                difference_type buffer_remaining = buffer_end - temp.current;
                if (n < buffer_remaining) {
                    temp.current += n;
                    return temp;
                }
                n -= buffer_remaining;
                size_t buffer_count = n / deque_buffer_size();
                n %= deque_buffer_size();
                temp.set_node(temp.node + buffer_count + 1);
                temp.current = temp.buffer_start + n;
            } else {
                n = -n;
                difference_type buffer_used = temp.current - temp.buffer_start;
                if (n <= buffer_used) {
                    temp.current -= n;
                    return temp;
                }
                n -= buffer_used;
                size_t buffer_count = (n - 1) / deque_buffer_size() + 1;
                n = (n - 1) % deque_buffer_size() + 1; 
                
                temp.set_node(temp.node - buffer_count);
                temp.current = temp.buffer_end - n;
            }
            return temp;
        }
        dequeIterator operator-(difference_type n) const {
            return *this + (-n);
        }
        difference_type operator-(const dequeIterator& other) const {
            return (node - other.node) * deque_buffer_size() + (current - buffer_start) - (other.current - other.buffer_start);
        }

        bool operator==(const dequeIterator& other) const { return current == other.current; }
        bool operator!=(const dequeIterator& other) const { return !(*this == other); }
    };


    template<typename T>
    class deque {
        public:
        using iterator = dequeIterator<T>;
        using const_iterator = dequeIterator<T, true>;
        using pointer = T*;
        using reference = T&;
        using const_pointer = const T*;
        using const_reference = const T&;
        using buffer_pointer = pointer*;
        
        using map_allocator = MyStl::allocator<pointer>;
        using data_allocator = MyStl::allocator<T>;
        private:
        buffer_pointer map; // 指向缓冲区指针的指针
        size_t map_size; // map的大小（缓冲区指针的数量）
        iterator start; // 指向第一个元素的迭代器
        iterator finish; // 指向最后一个元素的迭代器

        public:
        deque() {
            map = map_allocator::allocate(8); // 初始分配8个缓冲区指针
            for (int i = 0; i < 8; ++i) map[i] = nullptr;
            map_size = 8;

            pointer buffer = data_allocator::allocate(deque_buffer_size()); // 分配第一个缓冲区
            for (size_t i = 0; i < deque_buffer_size(); ++i) {
                data_allocator::construct(buffer + i);
            }

            map[3] = buffer; // 将第4个缓冲区指针指向第一个缓冲区
            start = iterator(map + 3, buffer + deque_buffer_size() / 2); // 从缓冲区中间开始
            finish = iterator(map + 3, buffer + deque_buffer_size() / 2); // 初始时没有元素
        }

        iterator begin() { return start; }
        iterator end() { return finish; }
        
        ~deque() {
            // 释放所有已分配的缓冲区
            for (size_t i = 0; i < map_size; ++i) {
                if (map[i] != nullptr) {
                    for (size_t j = 0; j < deque_buffer_size(); ++j) {
                        data_allocator::destroy(map[i] + j);
                    }
                    data_allocator::deallocate(map[i], deque_buffer_size());
                }
            }
            // 释放 map 本身
            map_allocator::deallocate(map, map_size);
        }

        const_iterator begin() const { return start; }
        const_iterator end() const { return finish; }
        const_iterator cbegin() const { return start; }
        const_iterator cend() const { return finish; }
        bool empty() const { return start == finish; }

        size_t size() const {
            return finish - start;
        }

        reference operator[](size_t n) {
            return *(start + n);
        }

        const_reference operator[](size_t n) const {
            return *(start + n);
        }

        void expand() {
            // 1. 算出真实用掉的 Buffer 个数 (千万别算成元素个数！)
            size_t used_nodes = finish.node - start.node + 1;

            // 2. 申请两倍大的新 Map，并自动初始化为空指针
            size_t new_map_size = map_size * 2;
            buffer_pointer new_map = map_allocator::allocate(new_map_size);
            for (size_t i = 0; i < new_map_size; ++i) new_map[i] = nullptr;

            // 3. 计算居中对齐的起始下标
            size_t new_start_idx = (new_map_size - used_nodes) / 2;

            // 4. 极速搬家：只搬指针，不搬数据！
            for (size_t i = 0; i < used_nodes; ++i) {
                new_map[new_start_idx + i] = start.node[i];
            }

            // 5. 销毁旧 Map，避免内存泄漏
            map_allocator::deallocate(map, map_size);
            
            // 6. 更新容器状态
            map = new_map;
            map_size = new_map_size;

            // 7. 终极优雅：迭代器只需要更新它的 Map 坐标 (node)，
            // 它的 current, buffer_start, buffer_end 依然绝对正确，一个字都不用改！
            start.node = map + new_start_idx;
            finish.node = map + new_start_idx + used_nodes - 1;
        }

        void push_back(const T& value) {
            *finish = value; // 直接利用重载的 operator*，连 current 都不用写！
            
            // 检查是否到了当前 Buffer 的最后一个格子
            if (finish.current + 1 == finish.buffer_end) {
                if (finish.node + 1 == map + map_size) {
                    expand(); // Map 满了，触发中控器扩容
                }
                pointer new_buffer = data_allocator::allocate(deque_buffer_size());
                for (size_t i = 0; i < deque_buffer_size(); ++i) {
                    data_allocator::construct(new_buffer + i);
                }
                *(finish.node + 1) = new_buffer; 
            }
            ++finish; 
        }

        void push_front(const T& value) {
            // 检查是否在当前 Buffer 的最左边
            if (start.current == start.buffer_start) {
                if (start.node == map) {
                    expand();
                }
                // 核心：只管修路！
                pointer new_buffer = data_allocator::allocate(deque_buffer_size());
                for (size_t i = 0; i < deque_buffer_size(); ++i) {
                    data_allocator::construct(new_buffer + i);
                }
                *(start.node - 1) = new_buffer;
            }
            --start; 
            *start = value; 
        }
    };
}