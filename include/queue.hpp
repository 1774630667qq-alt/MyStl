#include "list.hpp"

namespace MyStl {
    template<
        typename T,
        typename Container = MyStl::list<T> >
    class queue {
        protected:
        Container c;

        public:

        void push(const T& value) {
            c.push_back(value);
        }

        void pop() {
            c.pop_front();
        }

        typename Container::reference front() {
            return c.front();
        }

        typename Container::const_reference front() const {
            return c.front();
        }

        typename Container::reference back() {
            return c.back();
        }

        typename Container::const_reference back() const {
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