/*
 * @Author: Zhang YuHua 1774630667@qq.com
 * @Date: 2026-03-12 19:19:05
 * @LastEditors: Zhang YuHua 1774630667@qq.com
 * @LastEditTime: 2026-03-12 21:02:17
 * @FilePath: /MyStl/include/set.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE   
 */
#pragma once
#include "rb_tree.hpp"
#include "functional.hpp"

namespace MyStl
{
    template<typename T,
            typename Compare = MyStl::less<T>>
    class set {
        public:
        using value_type = T;
        using rep_type = MyStl::rb_tree<T, T, MyStl::identity<T>, Compare>;
        using iterator = typename rep_type::const_iterator;
        using const_iterator = typename rep_type::const_iterator;
        using difference_type = typename rep_type::difference_type;
        using pointer = T*;
        using reference = T&;
        using const_reference = const T&;
        using const_pointer = const T*;

        private:
        rep_type rep;

        public:
        set() : rep() {}

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

        const_iterator find(const T& value) {
            return rep.find(value);
        }

        pair<iterator, bool> insert(const T& value) {
            MyStl::pair<typename rep_type::iterator, bool> p = rep.insert_unique(value);
            return MyStl::make_pair(p.first, p.second);
        }

        void erase(const_iterator pos) {
            rep.erase(pos);
        }

        void clean() {
            rep.clean();
        }

        ~set() {}
    };
} // namespace MyStl
