#include "vector.hpp"

namespace MyStl {
    template<
        typename T,
        typename Container = MyStl::vector<T> >
    class stack {
        protected:
            Container c;
        
        public:
        void push(const T& val) {
            c.emplace_back(val);
        }

        void pop() {
            c.pop_back();
        }

        typename Container::reference top() {
            return c.back();
        }

        typename Container::const_reference top() const {
            return c.back();
        }

        bool empty() const {
            return c.empty();
        }

        size_t size() const {
            return c.size();
        }
    };
}