#pragma once
#include <cstddef> // for std::ptrdiff_t

namespace MyStl {
    // 1. 输入迭代器：只能读，不能写，只能单向移动（e.g. istream_iterator）
    struct input_iterator_tag {};

    // 2. 输出迭代器：只能写，不能读，只能单向移动（e.g. ostream_iterator）
    struct output_iterator_tag {};

    // 3. 前向迭代器：可以读写，可以单向移动（e.g. forward_list 的迭代器）
    // 继承自 input_iterator_tag，表示它也是输入迭代器
    struct forward_iterator_tag : input_iterator_tag {};

    // 4. 双向迭代器：可以读写，可以双向移动（e.g. list 的迭代器）
    // 继承自 forward_iterator_tag，表示它也是前向迭代器
    struct bidirectional_iterator_tag : forward_iterator_tag {};

    // 5. 随机访问迭代器：可以读写，可以双向移动，还可以进行随机访问（e.g. vector 的迭代器）
    // 继承自 bidirectional_iterator_tag，表示它也是双向迭代
    struct random_access_iterator_tag : bidirectional_iterator_tag {};

    template<typename Category,                 // 迭代器类型标签
            typename T,                         // 迭代器指向的元素类型
            typename Distance = std::ptrdiff_t, // 迭代器之间的距离类型，默认为 std::ptrdiff_t
            typename Pointer = T*,              // 迭代器指向的指针类型，默认为 T*
            typename Reference = T&>            // 迭代器指向的引用类型，默认为 T&
    struct iterator {
        using iterator_category = Category;
        using value_type        = T;
        using difference_type   = Distance;
        using pointer           = Pointer;
        using reference         = Reference;
    };

    // 迭代器萃取器：从迭代器类型中提取出上述五个成员类型
    template<typename Iterator>
    struct iterator_traits {
        using iterator_category = typename Iterator::iterator_category;     
        using value_type        = typename Iterator::value_type;
        using difference_type   = typename Iterator::difference_type;
        using pointer           = typename Iterator::pointer;
        using reference         = typename Iterator::reference;
    };

    // 偏特化版本：针对原生指针类型 T*
    template<typename T>
    struct iterator_traits<T*> {
        // 原生指针默认可以随机访问，所以迭代器类型标签是 random_access_iterator_tag
        using iterator_category = random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;
    };

    // 偏特化版本：针对 const 原生指针类型 const T*
    template<typename T>
    struct iterator_traits<const T*> {
        using iterator_category = random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const T*;
        using reference         = const T&;
    };

    // --- Distance ---

    // 版本 1: 针对随机访问迭代器 (O(1))
    // 只要是 RandomAccessIterator 或其子类，就会匹配到这里
    template<typename RandomAccessIterator>
    typename iterator_traits<RandomAccessIterator>::difference_type
    __distance(RandomAccessIterator first, RandomAccessIterator last, 
               random_access_iterator_tag) {
        return last - first;
    }

    // 版本 2: 针对输入迭代器 (O(N))
    // Input, Forward, Bidirectional 都会匹配到这里（因为继承关系）
    template<typename InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    __distance(InputIterator first, InputIterator last, 
               input_iterator_tag) {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) {
            ++first;
            ++n;
        }
        return n;
    }

    // 对外接口：自动萃取 Tag，然后分发给上面两个版本之一
    template<typename InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last) {
        using Category = typename iterator_traits<InputIterator>::iterator_category;
        return __distance(first, last, Category()); // <--- 魔法发生在这里！
    }

    // --- Advance ---

    // 版本 1: 随机访问 (O(1))
    template<typename RandomAccessIterator, typename Distance>
    void __advance(RandomAccessIterator& i, Distance n, 
                   random_access_iterator_tag) {
        i += n;
    }

    // 版本 2: 双向 (O(N)) - 支持负数 n
    template<typename BidirectionalIterator, typename Distance>
    void __advance(BidirectionalIterator& i, Distance n, 
                   bidirectional_iterator_tag) {
        if (n >= 0) 
            while (n--) ++i;
        else 
            while (n++) --i;
    }

    // 版本 3: 输入 (O(N)) - 只支持正数 n
    template<typename InputIterator, typename Distance>
    void __advance(InputIterator& i, Distance n, 
                   input_iterator_tag) {
        while (n--) ++i;
    }

    // 对外接口
    template<typename InputIterator, typename Distance>
    void advance(InputIterator& i, Distance n) {
        using Category = typename iterator_traits<InputIterator>::iterator_category;
        __advance(i, n, Category());
    }

    // --- 反向迭代器适配器 ---
    template <typename Iterator>
    class reverse_iterator {
    public:
        // 1. 利用 iterator_traits 提取原始迭代器的类型信息
        using iterator_category = typename iterator_traits<Iterator>::iterator_category;
        using value_type        = typename iterator_traits<Iterator>::value_type;
        using difference_type   = typename iterator_traits<Iterator>::difference_type;
        using pointer           = typename iterator_traits<Iterator>::pointer;
        using reference         = typename iterator_traits<Iterator>::reference;

        using iterator_type     = Iterator; // 保存原始迭代器类型

    protected:
        Iterator current; // 核心：内部持有一个正向迭代器

    public:
        // 构造函数
        reverse_iterator() {}
        explicit reverse_iterator(iterator_type x) : current(x) {}

        // 拷贝构造（允许从普通 iterator 转为 const iterator，反之亦然，取决于 Iterator 的兼容性）
        template <typename U>
        reverse_iterator(const reverse_iterator<U>& other) : current(other.base()) {}

        // 取出内部的正向迭代器
        iterator_type base() const { return current; }

        // --- 核心操作：错位取值 ---
        reference operator*() const {
            Iterator tmp = current;
            return *--tmp; // 关键！取前一个位置的值
        }

        pointer operator->() const {
            return &(operator*());
        }

        // --- 移动操作：方向反转 ---
        
        // ++ 变 --
        reverse_iterator& operator++() {
            --current;
            return *this;
        }

        reverse_iterator operator++(int) {
            reverse_iterator temp = *this;
            --current;
            return temp;
        }

        // -- 变 ++
        reverse_iterator& operator--() {
            ++current;
            return *this;
        }

        reverse_iterator operator--(int) {
            reverse_iterator temp = *this;
            ++current;
            return temp;
        }

        // --- 随机访问操作 (+= 变 -=) ---
        reverse_iterator operator+(difference_type n) const {
            return reverse_iterator(current - n);
        }

        reverse_iterator& operator+=(difference_type n) {
            current -= n;
            return *this;
        }

        reverse_iterator operator-(difference_type n) const {
            return reverse_iterator(current + n);
        }

        reverse_iterator& operator-=(difference_type n) {
            current += n;
            return *this;
        }

        reference operator[](difference_type n) const {
            return *(*this + n);
        }
    };

    // --- 全局运算符重载 (比较 & 算术) ---

    // 比较操作需要反过来：物理地址越小，反向迭代器越"大"（越接近逻辑终点）
    template <typename Iterator>
    bool operator==(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
        return lhs.base() == rhs.base();
    }

    template <typename Iterator>
    bool operator!=(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
        return !(lhs == rhs);
    }

    template <typename Iterator>
    bool operator<(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
        return rhs.base() < lhs.base(); // 注意：反过来了
    }

    template <typename Iterator>
    bool operator>(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
        return rhs < lhs;
    }

    template <typename Iterator>
    bool operator<=(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
        return !(rhs < lhs);
    }

    template <typename Iterator>
    bool operator>=(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
        return !(lhs < rhs);
    }

    // 支持 n + it
    template <typename Iterator>
    reverse_iterator<Iterator> operator+(
        typename reverse_iterator<Iterator>::difference_type n,
        const reverse_iterator<Iterator>& x) {
        return reverse_iterator<Iterator>(x.base() - n);
    }

    // 两个反向迭代器相减 (物理 end - begin)
    template <typename Iterator>
    typename reverse_iterator<Iterator>::difference_type operator-(
        const reverse_iterator<Iterator>& lhs,
        const reverse_iterator<Iterator>& rhs) {
        return rhs.base() - lhs.base(); // 注意：反过来了
    }
}