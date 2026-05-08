#include <cstdint>
#include <format>
#include <iostream>
#include <new>

int main() {
    int x = 100;
    int* ptr = &x;
    *ptr = 200;
    // 注意：打印指针地址时，std::format 有时需要明确处理指针类型
    std::cout << std::format("*ptr: {}", *ptr) << '\n';
    std::cout << std::format("ptr: {}", ptr) << '\n';

    return 0;
}
