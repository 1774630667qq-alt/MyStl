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
        using key_type = K;
        using value_type = V;
        using Node = HashNode<K, V>;

        HashNodeBase Node_;                           // 实体哨兵节点！(不再是指针)
        MyStl::vector<HashNodeBase*> buckets;         // 桶数组存的全是父类指针
        Hash hasher_;                                 // 哈希函数对象
        size_t size_;                                 

        size_t bucket_index (const K& key) const {    
            return hasher_(key) % buckets.size();
        }

        public:
        unordered_map(): buckets(8, nullptr), size_(0) {}

        iterator insert(const K& key, const V& val) {
            if (size_ >= buckets.size()) {
                rehash(buckets.size() * 2);
            }
            
            size_t index = bucket_index(key);
            // 1. new 出带有真实数据的子类节点
            HashNode<K, V>* newNode = new HashNode<K, V>(MyStl::make_pair(key, val));

            if (buckets[index] == nullptr) {
                if (Node_.next != nullptr) {
                    // 这里好像错误了？
                    // HashNode<K, V>* old_first = static_cast<HashNode<K, V>*>(buckets[index]);
                    // 纠正，未插入前首个桶的首个节点应该是头节点的后继才对
                    HashNode<K, V>* old_first = static_cast<HashNode<K, V>*>(Node_.next);
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
                newNode->next = buckets[index]->next;
                buckets[index]->next = newNode;
                ++size_;
            }
            return iterator(newNode);
        }

        iterator begin() {
            return iterator(static_cast<HashNode<K, V>*>(Node_.next));
        }

        const_iterator begin() const {
            return const_iterator(static_cast<HashNode<K, V>*>(Node_.next));
        }

        iterator end() {
            return iterator(nullptr);
        }

        const_iterator end() const {
            return const_iterator(nullptr);
        }

        iterator find(const K& key) {
            size_t index = bucket_index(key);

            // 1. 安全第一：如果是空桶，直接返回 end()
            if (buckets[index] == nullptr) {
                return end();
            }

            // 2. 找到这个桶真正的第一位住户 (前驱的 next)，并转为子类指针
            HashNodeBase* first_base = static_cast<HashNodeBase*>(buckets[index]->next);
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

        void rehash(size_t new_bucket_count) {
            // 如果新桶数量还没现在多，直接忽略
            if (new_bucket_count <= buckets.size()) return;

            // 1. 准备一个全新的、更大的空桶数组
            MyStl::vector<HashNodeBase*> new_buckets(new_bucket_count, nullptr);
            
            // 2. 把旧的全局大链表“连根拔起”，保存在 curr 里
            HashNodeBase* curr = Node_.next;
            
            // 3. 将当前哈希表彻底“重置”为一个空表（但 size_ 保持不变）
            Node_.next = nullptr;
            // 我们之前写的移动赋值运算符派上用场了！O(1) 偷走新数组
            buckets = MyStl::move(new_buckets); 

            // 4. 重新洗牌：把旧链表里的节点，挨个摘下来，重新插入到新表里
            while (curr != nullptr) {
                // ==========================================
                // 🎯 你的 TODO：完成重新挂载
                // ==========================================
                
                // 步骤 A：提前保存下一个节点，因为稍后 curr->next 就要被改写了！(极其重要，否则会断链)
                HashNodeBase* next_node = curr->next;
                
                // 步骤 B：把 curr 转型为子类指针 (HashNode<K, V>*)，以便读取里面的 data.first (也就是 key)
                key_type key = static_cast<HashNode<K, V>*>(curr)->data.first;
                // 步骤 C：用这个 key，调用 bucket_index 算出它在扩容后的新桶下标。
                // (此时 buckets.size() 已经是 new_bucket_count 了，所以算出来的是全新的下标)
                size_t index = bucket_index(key);
                
                // 步骤 D：用你之前在 insert 里写的 if-else 逻辑（空桶头插 vs 局部头插），把 curr 插进新表。
                // (💡提示：直接把你刚才 insert 里的 new HashNode 替换成 curr 即可！完全不用 new！)
                // 修改pre的后继
                if (buckets[index] == nullptr) {
                    if (Node_.next != nullptr) { // 如果新链表不为空，说明之前插入过节点，那个节点是它所在桶的首节点
                        key_type temp_key = static_cast<HashNode<K, V>*>(Node_.next)->data.first;
                        size_t old_index = bucket_index(temp_key);
                        buckets[old_index] = curr;
                    }

                    curr->next = Node_.next;
                    Node_.next = curr;
                    buckets[index] = &Node_;
                } else { // 发生冲突
                    curr->next = buckets[index]->next;
                    buckets[index]->next = curr;
                }
                
                // 步骤 E：curr = next_node，继续处理下一个老住户。
                curr = next_node;
            }
        }

        V& operator[](const K& key) {
            // 1. 先查看当前这个 key 是否已经存在了，如果存在了就直接返回对应的 value 的引用
            if (iterator it = find(key); it != end()) {
                return it->second;
            }
            // 2. 如果不存在，就插入一个默认值，然后返回这个新插入的 value 的引用
            return insert(key, V())->second;
        }

        void clear() {
            HashNodeBase* curr = Node_.next;
            while (curr != nullptr) { // 清空单链表
                HashNodeBase* temp = curr->next;
                delete static_cast<HashNode<K, V>*>(curr);
                curr = temp;
            }
            Node_.next = nullptr; // 关键：哨兵节点必须指向空，否则是野指针

            buckets.assign(buckets.size(), nullptr); // 桶数组重置为全空
            
            size_ = 0;
        }

        size_t size() const {
            return size_;
        }

        ~unordered_map() {
            clear();
            // 这里不需要 delete Node_; 因为它是栈上的实体对象，会自动销毁！
        }
    };
}