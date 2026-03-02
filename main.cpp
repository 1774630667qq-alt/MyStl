#include<iostream>
#include "type_traits.hpp"
#include "utility.hpp"
#include "vector.hpp"
#include <string>
#include "function.hpp"
#include "list.hpp"

struct a{
    int key;
};

struct b : public a{
    int value;
    b(int k, int v) : a{k}, value(v) {}
};


int main() {
}