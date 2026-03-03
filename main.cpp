#include <iostream>
#include <deque>
int main() {
    std::deque<int> d;
    d.push_back(1);
    d.push_back(2);
    d.push_back(3);

    for (const auto& item : d) {
        std::cout << item << " ";
    }
    std::cout << std::endl;
    std::cout << d[1] << std::endl; // 输出 2

    return 0;
    
}