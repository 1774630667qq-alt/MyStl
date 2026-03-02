#include "vector.hpp"
#include "functional.hpp"

namespace MyStl {
    template<
        typename T,
        typename Container = MyStl::vector<T>,
        typename Compare = MyStl::less<T> >
    class priority_queue {
        private:
        Container c;
        Compare comp;

        public:
        priority_queue() : c(), comp() {}

        priority_queue(const Container& container) : c(container), comp() {
            if (c.size() < 2) return; // 0 或 1 个元素不需要堆化

            // 从最后一个非叶子节点开始，依次调整每个节点以满足堆性质
            // 使用 int 避免 size_t (unsigned) 在 0-1 时发生下溢
            for (int i = c.size() / 2 - 1; i >= 0; --i) {
                down(i);
            }
        }

        priority_queue(const Compare& compare) : c(), comp(compare) {}


        void push(const T& value) {
            c.emplace_back(value);
            size_t i = c.size() - 1;
            while (i > 0) {
                size_t parent = (i - 1) / 2;
                if (comp(c[parent], c[i])) { // 父节点小于当前节点，交换并继续调整
                    MyStl::swap(c[parent], c[i]);
                    i = parent;
                } else { // 父节点不小于当前节点，堆性质满足，停止调整
                    break;
                }
            }
        }

        const T& top() const {
            return c.front();
        }

        void down(size_t i = 0) { 
            while (i < c.size()) {
                size_t l_child = 2 * i + 1, r_child = 2 * i + 2;
                size_t largest = i; // 假设当前节点是最大的
                if (l_child < c.size() && comp(c[largest], c[l_child])) {
                    largest = l_child; // 左子节点更大
                }
                if (r_child < c.size() && comp(c[largest], c[r_child])) {
                    largest = r_child; // 右子节点更大
                }
                if (largest != i) { // 如果最大的不是当前节点，交换并继续调整
                    MyStl::swap(c[i], c[largest]);
                    i = largest;
                } else { // 如果最大的就是当前节点，堆性质满足，停止调整
                    break;
                }
            }
        }

        void pop() {
            // 将堆顶元素与最后一个元素交换，然后删除最后一个元素
            MyStl::swap(c.front(), c.back());
            c.pop_back();
            down();
        }

        bool empty() const {
            return c.empty();
        }
    };
}