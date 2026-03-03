#include <cstddef>

namespace MyStl {
    inline size_t deque_buffer_size() {
        return 64; // 每个缓冲区存放64个元素
    }

    template<typename T, bool IsConst = false>
    class dequeIterator {
        public:
        using iterator_category = MyStl::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = typename MyStl::conditional_t<IsConst, const T*, T*>;
        using reference = typename MyStl::conditional_t<IsConst, const T&, T&>;
        using pointer = T*;
        using reference = T&;
        using const_pointer = const T*;
        using const_reference = const T&;
        using buffer_pointer = pointer*; // 指向缓冲区指针的指针

        private:
        pointer current; // 当前元素的指针
        pointer buffer_start; // 当前缓冲区的起始地址
        pointer buffer_end; // 当前缓冲区的结束地址
        buffer_pointer node; // 指向缓冲区指针的指针

        public:
        
    }
}