#include <array>
#include <format>
#include <iostream>
#include <stdexcept>

int main() {
    std::array<int, 5> prices = {100, 101, 102, 103, 104};

    // 1. arr[i] 的典型用法：追求极致速度
    // 在 HFT 中，如果你已经通过逻辑保证 i 不会越界（例如循环边界），
    // 编译器会将此操作直接优化为单条机器指令。
    int fast_val = prices[2];
    std::cout << std::format("Fast access: {}\n", fast_val);

    // 2. arr.at(i) 的典型用法：宁愿报错也不要崩盘
    // 这多出的逻辑判断会破坏 CPU 的分支预测，增加纳秒级的延迟。
    try {
        int safe_val = prices.at(10); // 故意越界
        std::cout << std::format("Safe access: {}\n", safe_val);
    } catch (const std::out_of_range& e) {
        std::cerr << std::format("Caught: {}\n", e.what());
    }

    return 0;
}
