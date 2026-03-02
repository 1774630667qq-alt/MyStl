#include <iostream>
#include <string>
#include "vector.hpp"
#include "priority_queue.hpp"

// 辅助函数：打印测试结果
void assert_true(bool condition, const std::string& message) {
    if (condition) {
        std::cout << "[PASS] " << message << std::endl;
    } else {
        std::cout << "[FAIL] " << message << std::endl;
    }
}

int main() {
    std::cout << "=== Testing MyStl::priority_queue ===" << std::endl;

    // 1. 测试基本 Push 和 Top (最大堆)
    {
        MyStl::priority_queue<int> pq;
        pq.push(10);
        pq.push(5);
        pq.push(20);
        pq.push(1);

        assert_true(pq.top() == 20, "Top element should be 20");
        assert_true(!pq.empty(), "Queue should not be empty");
    }

    // 2. 测试 Pop 顺序 (堆排序效果)
    {
        MyStl::priority_queue<int> pq;
        pq.push(3);
        pq.push(1);
        pq.push(4);
        pq.push(1);
        pq.push(5);
        pq.push(9);

        std::cout << "Pop order (expect 9 5 4 3 1 1): ";
        int prev = 100; // 假设一个比所有元素都大的数
        bool order_correct = true;
        
        while (!pq.empty()) {
            int current = pq.top();
            std::cout << current << " ";
            if (current > prev) order_correct = false;
            prev = current;
            pq.pop();
        }
        std::cout << std::endl;
        assert_true(order_correct, "Elements popped in descending order");
        assert_true(pq.empty(), "Queue should be empty after popping all");
    }

    // 3. 测试通过 vector 构造 (Heapify 逻辑)
    {
        MyStl::vector<int> vec;
        vec.push_back(10);
        vec.push_back(30);
        vec.push_back(20);
        
        // 这里会触发 priority_queue 的构造函数中的 down 调整逻辑
        MyStl::priority_queue<int> pq(vec);
        
        assert_true(pq.top() == 30, "Heapify: Top should be 30");
        pq.pop();
        assert_true(pq.top() == 20, "Heapify: Next should be 20");
    }

    // 4. 测试边界情况：只有一个元素的 vector
    {
        MyStl::vector<int> vec_single;
        vec_single.push_back(42);
        
        // 如果构造函数逻辑有 bug (size_t 下溢)，这里可能会崩溃
        MyStl::priority_queue<int> pq(vec_single);
        
        assert_true(pq.top() == 42, "Single element constructor works");
        pq.pop();
        assert_true(pq.empty(), "Empty after popping single element");
    }

    // 5. 测试自定义类型 (std::string)
    {
        MyStl::priority_queue<std::string> pq_str;
        pq_str.push("apple");
        pq_str.push("banana");
        pq_str.push("cherry");
        
        assert_true(pq_str.top() == "cherry", "String comparison works (cherry > banana)");
    }

    return 0;
}