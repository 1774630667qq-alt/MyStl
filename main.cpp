#include<iostream>
#include "type_traits.hpp"
#include "utility.hpp"
#include "vector.hpp"
#include <string>
#include "function.hpp"
#include "list.hpp"
#include "undored_map.hpp"

int main() {
    MyStl::unordered_map<std::string, int> myMap;
    myMap.insert("hello world", 42);
    std::cout << myMap["hello world"] << std::endl; // 输出 42
    myMap["foo"] = 100;
    std::cout << myMap["foo"] << std::endl; // 输出 100
    std::cout << myMap["bar"] << std::endl; // 输出 0（默认值）
    return 0;
}