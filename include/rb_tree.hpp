#pragma once
#include <cstddef>
#include "type_traits.hpp"
#include "iterator.hpp"
#include "utility.hpp"
#include "functional.hpp"

namespace MyStl {
    // 1. 颜色类型定义
    using rb_tree_color_type = bool;
    constexpr rb_tree_color_type rb_tree_red = false;
    constexpr rb_tree_color_type rb_tree_black = true;

    // 2. 基础节点 Base Node
    struct rb_tree_node_base {
        using base_ptr = rb_tree_node_base *;

        rb_tree_color_type color; // 红色或黑色
        base_ptr parent; // 父节点
        base_ptr left; // 左子节点
        base_ptr right; // 右子节点

        // 提示：你可以在这里写一个获取最小/最大节点的静态辅助函数
        // 找以 x 为根的子树的最小节点（一直往左走）
        static base_ptr minimum(base_ptr x) {
            while (x->left != nullptr) x = x->left;
            return x;
        }

        // 找以 x 为根的子树的最大节点（一直往右走）
        static base_ptr maximum(base_ptr x) {
            while (x->right != nullptr) x = x->right;
            return x;
        }
    };

    // 3. 泛型数据节点 Data Node
    template<typename T>
    struct rb_tree_node : public rb_tree_node_base {
        T value_field; // 真正的泛型数据
        rb_tree_node() {
            parent = nullptr;
            left = nullptr;
            right = nullptr;
            color = rb_tree_red; // 默认红色
            value_field = T();
        }

        rb_tree_node(const T &value) : value_field(value) {
            parent = nullptr;
            left = nullptr;
            right = nullptr;
            color = rb_tree_red; // 默认红色
        }

        rb_tree_node &operator=(const rb_tree_node &other) {
            if (this != &other) {
                value_field = other.value_field;
                color = other.color;
                parent = other.parent;
                left = other.left;
                right = other.right;
            }
            return *this;
        }
    };

    template<typename Key,
            typename T,
            typename KeyOfValue,
            typename Compare = MyStl::less<Key>>
    class rb_tree;

    template<typename T, bool IsConst = false>
    class rb_tree_iterator {
        template<typename Key, typename Val, typename KeyOfValue, typename Compare>
        friend class rb_tree;

        friend class rb_tree_iterator<T, !IsConst>;

    public:
        using iterator_category = MyStl::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = MyStl::conditional_t<IsConst, const T *, T *>;
        using reference = MyStl::conditional_t<IsConst, const T &, T &>;
        using Node = rb_tree_node_base;
        using link_type = Node *;

    private:
        link_type node;

    public:
        rb_tree_iterator() : node(nullptr) {
        }

        explicit rb_tree_iterator(link_type x) : node(x) {
        }

        template<bool OtherConst, typename = MyStl::enable_if_t<IsConst != OtherConst>>
        rb_tree_iterator(const rb_tree_iterator<T, OtherConst>& other) : node(other.node) {}

        reference operator*() const {
            return static_cast<rb_tree_node<T> *>(node)->value_field;
        }

        pointer operator->() const {
            return &(operator*());
        }

        rb_tree_iterator &operator++() {
            if (node->right != nullptr) {
                node = Node::minimum(node->right);
            } else {
                link_type y = static_cast<link_type>(node->parent);
                while (node == y->right) {
                    node = y;
                    y = static_cast<link_type>(y->parent);
                }
                if (node->right != y) {
                    // 必须保留的魔法拦截！
                    node = y;
                }
            }
            return *this;
        }

        rb_tree_iterator operator++(int) {
            rb_tree_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        rb_tree_iterator &operator--() {
            if (node->color == rb_tree_red && node->parent->parent == node) {
                node = static_cast<link_type>(node->right); // 从 end() 退回最大节点
            } else if (node->left != nullptr) {
                node = Node::maximum(node->left);
            } else {
                link_type y = static_cast<link_type>(node->parent);
                while (node == y->left) {
                    node = y;
                    y = static_cast<link_type>(y->parent);
                }
                node = y;
            }
            return *this;
        }

        rb_tree_iterator operator--(int) {
            rb_tree_iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const rb_tree_iterator &other) const {
            return node == other.node;
        }

        bool operator!=(const rb_tree_iterator &other) const {
            return node != other.node;
        }
    };

    template<typename Key,
            typename T,
            typename KeyOfValue,
            typename Compare>
    class rb_tree {
    public:
        using value_type = T;
        using iterator = rb_tree_iterator<T, false>;
        using const_iterator = rb_tree_iterator<T, true>;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;
        using const_pointer = const T *;
        using const_reference = const T &;
        using Node = rb_tree_node_base;
        using link_type = Node *;

    private:
        link_type header; // header 节点
        size_t node_count; // 树中节点数量
        Compare comp;
        KeyOfValue key_get;

    public:
        rb_tree(const Compare& c = Compare()) : node_count(0), comp(c), key_get() {
            header = new Node();
            header->color = rb_tree_red; // 将哨兵节点硬编码为红色
            header->parent = nullptr;
            header->left = header; // left 指向最小节点
            header->right = header; // right 指向最大节点
        }

        iterator begin() {
            return iterator(header->left); // 最小节点
        }

        iterator end() {
            return iterator(header);
        }

        const_iterator begin() const {
            return const_iterator(header->left); // 最小节点
        }

        const_iterator end() const {
            return const_iterator(header);
        }

        size_t size() const {
            return node_count;
        }

        bool empty() const {
            return node_count == 0;
        }

        iterator lower_bound(const Key &key) {
            link_type y = header;
            link_type x = header->parent;

            while (x != nullptr) {
                if (comp(key_get(static_cast<rb_tree_node<T> *>(x)->value_field), key)) {
                    x = x->right; 
                } else {
                    y = x;
                    x = x->left;
                }
            }
            return iterator(y);
        }

        iterator upper_bound(const Key &key) {
            link_type y = header;
            link_type x = header->parent;

            while (x != nullptr) {
                if (comp(key, key_get(static_cast<rb_tree_node<T> *>(x)->value_field))) {
                    y = x;
                    x = x->left;
                } else {
                    x = x->right;
                }
            }
            return iterator(y);
        }

        MyStl::pair<iterator, iterator> equal_range(const Key &key) {
            return MyStl::make_pair(lower_bound(key), upper_bound(key));
        }

        iterator find(const Key &key) {
            link_type y = header; // 最后一个不小于 value 的节点
            link_type x = header->parent; // 根节点
            while (x != nullptr) {
                if (comp(key_get(static_cast<rb_tree_node<T> *>(x)->value_field), key))
                    x = x->right;
                else {
                    y = x;
                    x = x->left;
                }
            }
            iterator j = iterator(y);
            return (j == end() || comp(key, key_get(*j))) ? end() : j;
        }

        /**
         * @author: Bazinga
         * @description: 左旋转（绕 x 节点进行左旋）
         * @param {link_type} x 旋转的中心节点
         * @return {*}
         */
        void left_rotate(link_type x) {
            link_type y = x->right;
            x->right = y->left;
            if (y->left != nullptr) {
                y->left->parent = x;
            }
            y->parent = x->parent;
            if (x->parent == header) {
                // x 是根节点
                header->parent = y;
            } else {
                if (x == x->parent->left) {
                    // x 是左孩子
                    x->parent->left = y;
                } else {
                    // x 是右孩子
                    x->parent->right = y;
                }
            }
            x->parent = y;
            y->left = x;
        }

        /**
         * @author: Bazinga
         * @description: 右旋转（绕 x 节点进行右旋）
         * @param {link_type} x 旋转的中心节点
         * @return {*}
         */
        void right_rotate(link_type x) {
            link_type y = x->left;
            x->left = y->right;
            if (y->right != nullptr) {
                y->right->parent = x;
            }
            y->parent = x->parent;
            if (x->parent == header) {
                // x 是根节点
                header->parent = y;
            } else {
                if (x == x->parent->right) {
                    // x 是右孩子
                    x->parent->right = y;
                } else {
                    // x 是左孩子
                    x->parent->left = y;
                }
            }
            x->parent = y;
            y->right = x;
        }

        void insert_fixup(link_type z) {
            while (z->parent != header && z->parent->color == rb_tree_red) {
                // error 1: 根节点进入会引发死循环
                if (z->parent == z->parent->parent->left) {
                    link_type y = z->parent->parent->right; // 叔叔节点
                    if (y != nullptr && y->color == rb_tree_red) {
                        // Case 1: 叔叔和父亲都为红色
                        z->parent->color = rb_tree_black;
                        y->color = rb_tree_black;
                        z->parent->parent->color = rb_tree_red;
                        z = z->parent->parent; // 向上修复
                    } else {
                        if (z == z->parent->right) {
                            // Case 2: 通过左旋转化成 Case 3
                            z = z->parent;
                            left_rotate(z);
                        }
                        z->parent->color = rb_tree_black;
                        z->parent->parent->color = rb_tree_red;
                        right_rotate(z->parent->parent);
                    }
                } else {
                    link_type y = z->parent->parent->left; // 叔叔节点
                    if (y != nullptr && y->color == rb_tree_red) {
                        z->parent->color = rb_tree_black;
                        y->color = rb_tree_black;
                        z->parent->parent->color = rb_tree_red;
                        z = z->parent->parent;
                    } else {
                        if (z == z->parent->left) {
                            z = z->parent;
                            right_rotate(z);
                        }
                        z->parent->color = rb_tree_black;
                        z->parent->parent->color = rb_tree_red;
                        left_rotate(z->parent->parent);
                    }
                }
            }
            header->parent->color = rb_tree_black; // error 2: 强效补丁，根节点必须为黑色
        }

        iterator insert_equal(const T &value) {
            link_type y = header; // 最后一个不小于 value 的节点
            link_type x = header->parent; // 根节点
            while (x != nullptr) {
                y = x;
                if (comp(key_get(static_cast<rb_tree_node<T> *>(x)->value_field), key_get(value))) {
                    x = x->right;
                } else {
                    x = x->left;
                }
            }
            link_type new_node = static_cast<link_type>(new rb_tree_node<T>(value));
            if (y == header) {
                // 树是空的
                header->parent = new_node;
                header->left = new_node;
                header->right = new_node;
                new_node->parent = header;
                new_node->color = rb_tree_black; // 根节点必须是黑色
            } else {
                if (comp(key_get(static_cast<rb_tree_node<T> *>(y)->value_field), key_get(value))) {
                    y->right = new_node;
                    if (header->right == y) {
                        header->right = new_node; // 更新最大节点
                    }
                } else {
                    y->left = new_node;
                    if (header->left == y) {
                        header->left = new_node; // 更新最小节点
                    }
                }
                new_node->parent = y;
            }
            node_count++;
            insert_fixup(new_node);
            return iterator(new_node);
        }

        MyStl::pair<iterator, bool> insert_unique(const T &value) {
            link_type y = header;
            link_type x = header->parent; // 根节点
            
            // 查重与寻路同时进行
            while (x != nullptr) {
                y = x;
                if (comp(key_get(value), key_get(static_cast<rb_tree_node<T> *>(x)->value_field))) {
                    // value 严格小于 x，向左走
                    x = x->left;
                } else if (comp(key_get(static_cast<rb_tree_node<T> *>(x)->value_field), key_get(value))) {
                    // x 严格小于 value，向右走
                    x = x->right;
                } else {
                    // 既不小于，也不大于 => 绝对相等！
                    // 查重成功！直接在半路拦截，返回已存在的节点和 false
                    return MyStl::pair<iterator, bool>(iterator(x), false);
                }
            }
            
            // 如果能活着走出 while 循环，说明绝对没有重复元素！
            // 并且 y 已经稳稳地停在了新节点应该挂载的父节点上！
            
            link_type new_node = static_cast<link_type>(new rb_tree_node<T>(value));
            if (y == header) { // 空树特判
                header->parent = new_node;
                header->left = new_node;
                header->right = new_node;
                new_node->parent = header;
                new_node->color = rb_tree_black;
            } else {
                // 挂载新节点（只需判断 value 是该挂在左边还是右边）
                if (comp(key_get(value), key_get(static_cast<rb_tree_node<T> *>(y)->value_field))) {
                    y->left = new_node;
                    if (header->left == y) {
                        header->left = new_node;
                    }
                } else {
                    y->right = new_node;
                    if (header->right == y) {
                        header->right = new_node;
                    }
                }
                new_node->parent = y;
            }
            
            node_count++;
            insert_fixup(new_node);
            return MyStl::pair<iterator, bool>(iterator(new_node), true);
        }

        // 必须要传递一个父节点，因为由于x可能为nullptr，无法通过空指针找到其父节点
        void erase_fixup(link_type x, link_type x_parent) {
            // 只要 x 不是根节点，且 x 身上没有可以用来填坑的红色
            while (x_parent != header && (x == nullptr || x->color == rb_tree_black)) {
                if (x == x_parent->left) {
                    link_type w = x_parent->right; // 兄弟节点
                    if (w->color == rb_tree_red) { // 情况 1
                        w->color = rb_tree_black;
                        x_parent->color = rb_tree_red;
                        left_rotate(x_parent); // 绝对不能用 x->parent!
                        w = x_parent->right;
                    }
                    if ((w->left == nullptr || w->left->color == rb_tree_black) &&
                        (w->right == nullptr || w->right->color == rb_tree_black)) { // 情况 2
                        w->color = rb_tree_red;
                        x = x_parent; // 甩锅给父亲
                        x_parent = x_parent->parent; // 父亲继续往上爬
                    } else {
                        if (w->right == nullptr || w->right->color == rb_tree_black) { // 情况 3
                            if (w->left != nullptr) w->left->color = rb_tree_black;
                            w->color = rb_tree_red;
                            right_rotate(w);
                            w = x_parent->right;
                        }
                        // 情况 4
                        w->color = x_parent->color;
                        x_parent->color = rb_tree_black;
                        if (w->right != nullptr) w->right->color = rb_tree_black;
                        left_rotate(x_parent);
                        x = header->parent; // 直接设为根节点，打破循环 (注意是 header->parent)
                    }
                } else { // x 是右孩子
                    link_type w = x_parent->left; // 注意这里是 ->
                    if (w->color == rb_tree_red) { // 情况 1
                        w->color = rb_tree_black;
                        x_parent->color = rb_tree_red;
                        right_rotate(x_parent);
                        w = x_parent->left;
                    }
                    if ((w->right == nullptr || w->right->color == rb_tree_black) &&
                        (w->left == nullptr || w->left->color == rb_tree_black)) { // 情况 2
                        w->color = rb_tree_red;
                        x = x_parent;
                        x_parent = x_parent->parent;
                    } else {
                        if (w->left == nullptr || w->left->color == rb_tree_black) { // 情况 3
                            if (w->right != nullptr) w->right->color = rb_tree_black;
                            w->color = rb_tree_red;
                            left_rotate(w);
                            w = x_parent->left;
                        }
                        // 情况 4
                        w->color = x_parent->color;
                        x_parent->color = rb_tree_black;
                        if (w->left != nullptr) w->left->color = rb_tree_black;
                        right_rotate(x_parent);
                        x = header->parent; // 打破循环
                    }
                }
            }
            if (x != nullptr) x->color = rb_tree_black; // 统一处理红色 x 或收尾
        }
        
        iterator erase(iterator pos)
        {
            link_type z = pos.node;
            link_type y = z;
            link_type x = nullptr;
            link_type x_parent = nullptr;

            iterator result = pos;
            ++result;

            if (y->left == nullptr) {
                x = y->right;
            } else if (y->right == nullptr) {
                x = y->left;
            } else {
                y = Node::minimum(y->right);
                x = y->right;
            }

            if (y != z) {
                z->left->parent = y;
                y->left = z->left;

                if (y != z->right) {
                    x_parent = y->parent;
                    if (x) x->parent = y->parent;
                    y->parent->left = x;
                    y->right = z->right;
                    z->right->parent = y;
                } else {
                    x_parent = y;
                }

                if (header->parent == z) {
                    header->parent = y;
                } else if (z->parent->left == z) {
                    z->parent->left = y;
                } else {
                    z->parent->right = y;
                }
                
                y->parent = z->parent;
                
                MyStl::swap(y->color, z->color);
            } else {
                x_parent = y->parent;
                if (x) x->parent = y->parent;

                if (z->parent == header) {
                    header->parent = x;
                } else if (z == z->parent->left) {
                    z->parent->left = x;
                } else {
                    z->parent->right = x;
                }

                if (header->left == z) {
                    if (z->right == nullptr) header->left = z->parent;
                    else header->left = Node::minimum(x);
                }
                if (header->right == z) {
                    if (z->left == nullptr) header->right = z->parent;
                    else header->right = Node::maximum(x);
                }
            }
            if (z->color == rb_tree_black) {
                erase_fixup(x, x_parent);
            }
            delete z;
            node_count--;
            return result;
        }

        void clean()
        {
            link_type x = header->parent;
            while (x != nullptr) {
                if (x->left != nullptr) {
                    x = x->left;
                } else if (x->right != nullptr) {
                    x = x->right;
                } else {
                    link_type y = x->parent;
                    if (y == header) {
                        delete static_cast<rb_tree_node<T>*>(x);
                        x = nullptr;
                    } else {
                        if (y->left == x) y->left = nullptr;
                        else y->right = nullptr;
                        delete static_cast<rb_tree_node<T>*>(x);
                        x = y;
                    }
                }
            }
            header->parent = nullptr;
            header->left = header;
            header->right = header;
            node_count = 0;
        }

        ~rb_tree()
        {
            clean();
            delete header;
        }
    };
}
