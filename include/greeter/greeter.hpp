#pragma once

#include <string>
#include <string_view>

namespace greeter {

// 返回一个格式化好的问候字符串。
// 使用 std::string_view 作为入参，避免调用侧被迫传 std::string
// 而产生一次不必要的拷贝（HFT 里能省一次 allocation 就省一次）。
std::string greet(std::string_view name);

} // namespace greeter
