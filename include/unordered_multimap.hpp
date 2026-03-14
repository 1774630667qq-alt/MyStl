/*
 * @Author: Zhang YuHua 1774630667@qq.com
 * @Date: 2026-03-14 17:57:50
 * @LastEditors: Zhang YuHua 1774630667@qq.com
 * @LastEditTime: 2026-03-14 18:03:36
 * @FilePath: /MyStl/include/unordered_multimap.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include "hashtable.h"
#include "utility.hpp"
#include "functional.hpp"
#include "allocator.hpp"

namespace MyStl {
    template<typename Key, 
            typename Value, 
            typename HashFcn = MyStl::hash<Key>, 
            typename EqualKey = MyStl::equal_to<Key>, 
            typename Alloc = MyStl::allocator<MyStl::pair<const Key, Value>>>
    class unordered_multimap {
        public:
        using value_type = MyStl::pair<const Key, Value>;
        using key_type = Key;
        using HTable = hashtable<value_type, Key, HashFcn, identity<Key>, EqualKey, Alloc>;
        using iterator = typename HTable::iterator;
        using const_iterator = typename HTable::const_iterator;
        using difference_type = typename HTable::difference_type;
        using pointer = value_type*;
        using reference = value_type&;
        using const_pointer = const value_type*;
        using const_reference = const value_type&;

        private:
        HTable rep;

        public:
        unordered_multimap() : rep() {}
        const_iterator begin() const { return rep.begin(); }
        const_iterator end() const { return rep.end(); }
        size_t size() const { return rep.size(); }
        bool empty() const { return rep.empty(); }
        const_iterator find(const Key& key) { return rep.find(key); }
        MyStl::pair<iterator, bool> insert(const value_type& value) { return rep.insert_unique(value); }
        MyStl::pair<iterator, iterator> equal_range(const Key& key) { return rep.equal_range(key); }
        void erase(const_iterator pos) { rep.erase(pos); }
        void clean() { rep.clear(); }
        ~unordered_multimap() {}
    };
}