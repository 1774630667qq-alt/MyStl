#include <iostream>
#include "deque.hpp"
#include "undored_map.hpp"

int main() {
    MyStl::deque<int> d;
    
    for (int i = 0; i < 10000; i++) {
        d.push_back(i + 1);
    }

    // for (const auto& item : d) {
    //     std::cout << item << " ";
    // }
    std::cout << d.size() << std::endl; // 输出 10000
    std::cout << d[9999] << std::endl; // 输出 10000

    MyStl::unordered_map<int, int> mp;
    for (int i = 1; i <= 10; i++) {
        mp[i] = i;
    }
    
    return 0;
    
}