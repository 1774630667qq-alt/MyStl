#include "construct.hpp"
#include <cstddef>

namespace MyStl{ 
    template<typename T>
    class allocator {
        public:
        using value_type      = T;
        using pointer         = T*;
        using const_pointer   = const T*;
        using reference       = T&;
        using const_reference = const T&;
        using size_type       = size_t;
        using difference_type = ptrdiff_t;

        public:
        static T* allocate(size_t n) {
            if (n == 0) return nullptr;
            return static_cast<T*>(::operator new(n * sizeof(T)));
        }

        static void deallocate(T* ptr, size_t /* n */) {
            if (ptr == nullptr) return;
            ::operator delete(ptr);
        }

        static void construct(T* ptr) {
            MyStl::construct(ptr);
        }

        template<typename... Args>
        static void construct(T* ptr, Args&&... args) {
            MyStl::construct(ptr, MyStl::forward<Args>(args)...);
        }

        static void destroy(T* ptr) {
            MyStl::destroy(ptr);
        }
        
        template<typename ForwordIterator>
        static void destroy(ForwordIterator first, ForwordIterator last) {
            MyStl::destroy(first, last);
        }
    };
}