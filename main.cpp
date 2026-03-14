/*
 * @Author: Zhang YuHua 1774630667@qq.com
 * @Date: 2026-02-16 10:11:06
 * @LastEditors: Zhang YuHua 1774630667@qq.com
 * @LastEditTime: 2026-03-14 16:30:13
 * @FilePath: /MyStl/main.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "set.hpp"
#include "map.hpp"
#include <iostream>
#include "function.hpp"
#include <unordered_map>


signed main() {
    std::unordered_map<int, int> mp;
    auto p = mp.insert(std::make_pair(1, 2));
    auto p1 = mp.insert(std::make_pair(1, 3));
    std::cout << p1.second << std::endl;
    return 0;
}
