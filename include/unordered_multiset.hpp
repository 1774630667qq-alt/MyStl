#pragma once
#include "hashtable.hpp"
#include "utility.hpp"
#include "functional.hpp"
#include "allocator.hpp"

namespace MyStl {
    template<typename T, 
            typename HashFcn = MyStl::hash<T>, 
            typename EqualKey = MyStl::equal_to<T>, 
            typename Alloc = MyStl::allocator<T>>
    class unordered_multiset {
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
        HTable h_table;

        public:
        unordered_multiset() : h_table() {}
        const_iterator begin() const { return h_table.begin(); }
        const_iterator end() const { return h_table.end(); }
        size_t size() const { return h_table.size(); }
        bool empty() const { return h_table.empty(); }
        const_iterator find(const T& value) { return h_table.find(value); }
        MyStl::pair<iterator, bool> insert(const T& value) { return h_table.insert_unique(value); }
        MyStl::pair<iterator, iterator> equal_range(const T& value) { return h_table.equal_range(value); }
        void erase(const_iterator pos) { h_table.erase(pos); }
        void clean() { h_table.clear(); }
        ~unordered_multiset() {}
    };
}