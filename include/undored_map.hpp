#include "utility.hpp"
#include <cstddef>
#include "vector.hpp"
#include <functional>

namespace MyStl {
    // 1. 父类（纯指针，非模板）
    struct HashNodeBase {
        HashNodeBase* next;
        HashNodeBase() : next(nullptr) {}
    };

    // 2. 子类（带数据，继承父类）
    template<typename K, typename V>
    struct HashNode : public HashNodeBase {
        MyStl::pair<const K, V> data;
        HashNode(const MyStl::pair<const K, V>& d) : data(d) {}
    };
    
    // 3. 迭代器
    template<typename K, typename V, bool IsConst = false> 
    class HashIterator {
        public:
        using T                 = MyStl::pair<const K, V>;
        using iterator_category = MyStl::forward_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = MyStl::conditional_t<IsConst, const T*, T*>;
        using reference         = MyStl::conditional_t<IsConst, const T&, T&>;

        private:
        using Node = HashNode<K, V>; // 迭代器持有的是子类指针！
        Node* ptr;

        public:
        HashIterator(Node* p = nullptr) : ptr(p) {} 

        reference operator*() const { return ptr->data; }
        pointer operator->() const { return &ptr->data; }

        HashIterator& operator++() {
            ptr = static_cast<Node*>(ptr->next);
            return *this;
        }
        HashIterator operator++(int) {
            HashIterator temp = *this;
            ptr = static_cast<Node*>(ptr->next);
            return temp;
        }
        bool operator==(const HashIterator& other) const { return ptr == other.ptr; }
        bool operator!=(const HashIterator& other) const { return ptr != other.ptr; }
    };

    // 4. 哈希表主体
    template<typename K, typename V, typename Hash = std::hash<K>> 
    class unordered_map {
        private:
        using iterator = HashIterator<K, V>;
        using const_iterator = HashIterator<K, V, true>;

        HashNodeBase Node_;                           // 实体哨兵节点！(不再是指针)
        MyStl::vector<HashNodeBase*> buckets;         // 桶数组存的全是父类指针
        Hash hasher_;                                 // 哈希函数对象
        size_t size_;                                 

        size_t bucket_index (const K& key) const {    
            return hasher_(key) % buckets.size();
        }

        public:
        unordered_map(): buckets(8, nullptr), size_(0) {}

        void insert(const K& key, const V& val) {
            size_t index = bucket_index(key);

            if (buckets[index] == nullptr) {
                // 1. new 出带有真实数据的子类节点
                HashNode<K, V>* newNode = new HashNode<K, V>(MyStl::make_pair(key, val));

                if (Node_.next == nullptr) {
                    HashNode<K, V>* old_first = static_cast<HashNode<K, V>*>(buckets[index]);
                    size_t old_index = bucket_index(old_first->data.first);
                    buckets[old_index] = newNode;
                }
                
                // 2. 头插法：连在哨兵节点 Node_ 的后面
                newNode->next = Node_.next;
                Node_.next = newNode;
                
                // 3. 记录前驱：因为接在 Node_ 后面，所以这个桶的前驱就是哨兵节点！
                buckets[index] = &Node_; 
                ++size_;
            }
            else { // 哈希冲突
                /*
                易错点：如果原本这个“前驱节点”也是其他桶的前驱，我们要不要更新那些桶？或者说，此时 buckets[index] 里的地址需要变吗？
                思考：这其中是否存在陷阱？难道一个节点不是至多被一个标记为前驱节点？确实是这样的，所以以上表述前一问大概是错误的，
                    这是一个思维误导？对于第二个问题，想来是不需要修改的，由于我们使用的是逻辑桶链表的头插，只要将原链表的前驱指向新的头节点即可
                    等等，什么时候桶会共用一个前驱，好像只有空插时会出现该问题也就是说另一个分支有问题。
                */
                HashNode<K, V>* newNode = new HashNode<K, V>(MyStl::make_pair(key, val));
                newNode->next = buckets[index]->next;
                buckets[index]->next = newNode;
                ++size_;
            }
        }

        typename iterator begin() {
            return iterator(static_cast<HashNode<K, V>*>(Node_.next));
        }

        typename const_iterator begin() const {
            return const_iterator(static_cast<HashNode<K, V>*>(Node_.next));
        }

        typename iterator end() {
            return iterator(nullptr);
        }

        typename const_iterator end() const {
            return const_iterator(nullptr);
        }

        iterator find(const K& key) {
            size_t index = bucket_index(key);

            // 1. 安全第一：如果是空桶，直接返回 end()
            if (buckets[index] == nullptr) {
                return end();
            }

            // 2. 找到这个桶真正的第一位住户 (前驱的 next)，并转为子类指针
            HashNodeBase* first_base = buckets[index]->next;
            Node* first_node = static_cast<Node*>(first_base);

            // 3. 极其清爽的 for 循环
            for (iterator it(first_node); it != end(); ++it) {
                
                // 越界检查：如果当前节点的 key 算出来的桶下标变了，说明已经跑到下一个桶了，及时止损
                if (bucket_index(it->first) != index) {
                    break;
                }

                // 找到了！
                if (it->first == key) {
                    return it;
                }
            }

            // 遍历完整个桶都没找到
            return end();
        }

        void clean() {}

        ~unordered_map() {
            clean();
            // 这里不需要 delete Node_; 因为它是栈上的实体对象，会自动销毁！
        }
    };
}