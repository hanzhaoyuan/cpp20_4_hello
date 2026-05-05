#include <greeter/greeter.hpp>

#include <format>

namespace greeter {

std::string greet(std::string_view name) {
    // std::format 是 C++20 引入的类型安全格式化设施，
    // 语义上等价于 Python 的 f-string、Rust 的 format!。
    // 对比 printf：编译期检查格式串、类型安全、无需手写 %d/%s。
    return std::format("Hello, {}! (built with C++20)", name);
}

} // namespace greeter
