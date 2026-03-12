#pragma once
#include "rb_tree.hpp"
#include "functional.hpp"

namespace MyStl {
    template<typename Key, typename Compare = MyStl::less<Key>>
    class multiset {
    public:
        using value_type = Key;
        using key_type = Key;
        // 底层引擎：使用 identity 提取键，且 Key 就是 Value
        using rep_type = MyStl::rb_tree<Key, Key, MyStl::identity<Key>, Compare>;
        
        // 【关键防御】迭代器强制绑定为常迭代器
        using iterator = typename rep_type::const_iterator;
        using const_iterator = typename rep_type::const_iterator;
        using difference_type = typename rep_type::difference_type;
        using pointer = Key*;
        using reference = Key&;
        using const_reference = const Key&;
        using const_pointer = const Key*;

    private:
        rep_type rep;

    public:
        multiset() : rep() {}

        iterator begin() const { return rep.begin(); }
        iterator end() const { return rep.end(); }
        size_t size() const { return rep.size(); }
        bool empty() const { return rep.empty(); }

        iterator find(const Key& key) { return rep.find(key); }

        iterator insert(const value_type& value) {
            return rep.insert_equal(value);
        }

        void erase(iterator pos) { rep.erase(pos); }
        void clean() { rep.clean(); }

        iterator lower_bound(const Key& key) { return rep.lower_bound(key); }
        iterator upper_bound(const Key& key) { return rep.upper_bound(key); }
        MyStl::pair<iterator, iterator> equal_range(const Key& key) {
            return rep.equal_range(key);
        }

        ~multiset() {}
    };
} // namespace MyStl