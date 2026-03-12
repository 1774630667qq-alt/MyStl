#pragma once
#include "rb_tree.hpp"
#include "functional.hpp"

namespace MyStl {
    template<typename Key, typename Value, typename Compare = MyStl::less<Key>>
    class multimap {
    public:
        // 【基因锁】Key 带有 const 属性，防止篡改！
        using value_type = MyStl::pair<const Key, Value>;
        using key_type = Key;
        // 底层引擎：使用 select1st 提取 first 作为比较的依据
        using rep_type = MyStl::rb_tree<Key, value_type, MyStl::select1st<value_type>, Compare>;
        
        // 普通迭代器和常迭代器各司其职
        using iterator = typename rep_type::iterator;
        using const_iterator = typename rep_type::const_iterator;
        using difference_type = typename rep_type::difference_type;
        using pointer = value_type*;
        using reference = value_type&;

    private:
        rep_type rep;

    public:
        multimap() : rep() {}

        iterator begin() { return rep.begin(); }
        iterator end() { return rep.end(); }
        const_iterator begin() const { return rep.begin(); }
        const_iterator end() const { return rep.end(); }

        size_t size() const { return rep.size(); }
        bool empty() const { return rep.empty(); }

        iterator find(const Key& key) { return rep.find(key); }

        iterator insert(const value_type& value) {
            return rep.insert_equal(value);
        }

        void erase(const_iterator pos) { rep.erase(pos); }
        void clean() { rep.clean(); }
        
        iterator lower_bound(const Key& key) { return rep.lower_bound(key); }
        iterator upper_bound(const Key& key) { return rep.upper_bound(key); }
        MyStl::pair<iterator, iterator> equal_range(const Key& key) {
            return rep.equal_range(key);
        }

        ~multimap() {}
    };
} // namespace MyStl