/*
 * @Author: Zhang YuHua 1774630667@qq.com
 * @Date: 2026-02-22 12:36:00
 * @LastEditors: Zhang YuHua 1774630667@qq.com
 * @LastEditTime: 2026-03-14 18:08:40
 * @FilePath: /MyStl/include/unordered_map.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AEtem
 */
#include "utility.hpp"
#include "hashtable.hpp"
#include <cstddef>
#include "vector.hpp"
#include "functional.hpp"
#include "allocator.hpp"

namespace MyStl {
    template <typename Key,
            typename Value,
            typename HashFcn = MyStl::hash<Key>, 
            typename EqualKey = MyStl::equal_to<Key>, 
            typename Alloc = MyStl::allocator<MyStl::pair<const Key, Value>>>
    class unordered_map {
        public:
        using value_type = MyStl::pair<const Key, Value>;
        using key_type = Key;
        using HTable = hashtable<value_type, Key, HashFcn, MyStl::select1st<value_type>, EqualKey, Alloc>;
        using iterator = typename HTable::iterator;
        using const_iterator = typename HTable::const_iterator;
        using difference_type = typename HTable::difference_type;
        using pointer = value_type*;
        using reference = value_type&;
        using const_pointer = const value_type*;
        using const_reference = const value_type&;

        private:
        HTable h_table;

        public:
        unordered_map() : h_table() {}
        
        Value& operator[](const key_type& k) {
            return insert(value_type(k, Value())).first->second;
        }
        
        const_iterator begin() const { return h_table.begin(); }
        const_iterator end() const { return h_table.end(); }
        size_t size() const { return h_table.size(); }
        bool empty() const { return h_table.empty(); }
        const_iterator find(const Key& key) { return h_table.find(key); }
        MyStl::pair<iterator, bool> insert(const value_type& value) { return h_table.insert_unique(value); }
        void erase(const_iterator pos) { h_table.erase(pos); }
        void clean() { h_table.clear(); }
        ~unordered_map() {}
    };
}