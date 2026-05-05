#pragma once

#include <string>
#include <string_view>

namespace greeter {

/// @brief 返回一个格式化好的问候字符串。
/// @param name 要问候的名字
/// @return 格式化后的问候字符串
std::string greet(std::string_view name);

} // namespace greeter
