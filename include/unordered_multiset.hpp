#pragma once
#include "hashtable.h"
#include "utility.hpp"
#include "functional.hpp"
#include "allocator.hpp"

namespace MyStl {
    template<typename T, 
            typename HashFcn = MyStl::hash<T>, 
            typename EqualKey = MyStl::equal_to<T>, 
            typename Alloc = MyStl::allocator<T>>
    class unordered_set {
        public:
        using value_type = T;
        using HTable = hashtable<T, T, HashFcn, identity<T>, EqualKey, Alloc>;
        using iterator = typename HTable::const_iterator;
        using const_iterator = typename HTable::const_iterator;
        using difference_type = typename HTable::difference_type;
        using pointer = T*;
        using reference = T&;
        using const_pointer = const T*;
        using const_reference = const T&;

        private:
        HTable rep;

        public:
        unordered_set() : rep() {}
        const_iterator begin() const { return rep.begin(); }
        const_iterator end() const { return rep.end(); }
        size_t size() const { return rep.size(); }
        bool empty() const { return rep.empty(); }
        const_iterator find(const T& value) { return rep.find(value); }
        MyStl::pair<iterator, bool> insert(const T& value) { return rep.insert_unique(value); }
        MyStl::pair<iterator, iterator> equal_range(const T& value) { return rep.equal_range(value); }
        void erase(const_iterator pos) { rep.erase(pos); }
        void clean() { rep.clear(); }
        ~unordered_set() {}
    };
}