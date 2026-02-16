#include<iostream>
#include "Vector.h"
#include <vector>


int main() {
    MyStl::Vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    std::cout << "随机访问遍历输出：" << std::endl;
    for (int i = 0;i < vec.size(); ++i) {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "迭代器遍历输出：" << std::endl;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "范围for循环遍历输出：" << std::endl;
    for (auto& it : vec) {
        std::cout << it << " ";
    }
    std::cout << std::endl;
}