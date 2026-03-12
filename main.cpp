/*
 * @Author: Zhang YuHua 1774630667@qq.com
 * @Date: 2026-02-16 10:11:06
 * @LastEditors: Zhang YuHua 1774630667@qq.com
 * @LastEditTime: 2026-03-12 21:26:21
 * @FilePath: /MyStl/main.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "set.hpp"
#include "map.hpp"
#include <iostream>


signed main() {
    MyStl::map<int, int> mp;
    mp[1] = 2;
    mp[4] = 3;
    mp[5] = 6;
    mp[3] = 4;
    for (auto [k, v]: mp) {
        std::cout << k << " " << v << std::endl;
    }
    return 0;
}
