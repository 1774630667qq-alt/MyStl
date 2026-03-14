/*
 * @Author: Zhang YuHua 1774630667@qq.com
 * @Date: 2026-03-12 21:04:12
 * @LastEditors: Zhang YuHua 1774630667@qq.com
 * @LastEditTime: 2026-03-12 21:29:30
 * @FilePath: /MyStl/include/map.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE   
 */
#pragma once
#include "rb_tree.hpp"
#include "functional.hpp"

namespace MyStl {
    template<typename Key, 
            typename Value,
            typename Compare = MyStl::less<Key>,
            typename Alloc = MyStl::allocator<MyStl::pair<const Key, Value>>>
    class map {
        public:
        using value_type = MyStl::pair<const Key, Value>;
        using key_type = Key;
        using rep_type = MyStl::rb_tree<Key, value_type, MyStl::select1st<value_type>, Compare, Alloc>;
        using iterator = typename rep_type::iterator;
        using const_iterator = typename rep_type::const_iterator;
        
        private:
        rep_type rep;

        public:
        map() : rep() {}

        Value& operator[](const key_type& k) {
            // insert_unique 返回一个 pair: {iterator to element, bool indicating insertion}
            // .first 是指向元素的迭代器，.second 是一个布尔值
            // 我们通过迭代器访问到 map 内部的 pair<const Key, Value>，并返回其 value 的引用
            return insert(value_type(k, Value())).first->second;
        }
        
        iterator begin() {
            return rep.begin();
        }

        iterator end() {
            return rep.end();
        }

        const_iterator begin() const {
            return rep.begin();
        }

        const_iterator end() const {
            return rep.end();
        }

        size_t size() const {
            return rep.size();
        }

        bool empty() const {
            return rep.empty();
        }

        iterator find(const key_type& k) {
            return rep.find(k);
        }

        MyStl::pair<iterator, bool> insert(const value_type& value) {
            return rep.insert_unique(value);
        }

        void erase(const_iterator pos) {
            rep.erase(pos);
        }

        void clean() {
            rep.clean();
        }

        ~map() {
        }
    };
}