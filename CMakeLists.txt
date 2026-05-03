# -----------------------------------------------------------------------------
# cpp20_4_hello —— 最小化的 C++20 + CMake 学习工程
#
# 目标：一个可执行文件 hello_app 链接一个独立静态库 greeter。
# 所有 target 都用"现代 CMake"(target-based) 写法，
# 方便以后平滑扩展到 HFT 多模块（order_book、matching_engine ...）。
# -----------------------------------------------------------------------------

# 需要 3.21+ 才能完整支持 CMakePresets.json v3
cmake_minimum_required(VERSION 3.21)

project(cpp20_4_hello
    VERSION 0.1.0
    LANGUAGES CXX
)

# -----------------------------------------------------------------------------
# 全局 C++ 标准设置（防御式）
# 作用：任何后续 add_library / add_executable 创建的 target 都默认使用 C++20。
# 之后每个 target 上还会再显式声明一次 target_compile_features，双保险。
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)   # 编译器不支持就报错，不要悄悄降级
set(CMAKE_CXX_EXTENSIONS OFF)         # 不用 GNU 扩展，坚持标准 C++

# 让 CMake 生成 compile_commands.json，clangd / Cursor 靠它做补全和跳转
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------------------------------------------------------------
# greeter 静态库
#   - 源文件只放 .cpp，头文件通过 target_include_directories 暴露
#   - include 目录标为 PUBLIC：自己用得到 + 链接它的 target 也能用
# -----------------------------------------------------------------------------
add_library(greeter STATIC
    src/greeter.cpp
)

target_include_directories(greeter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(greeter PUBLIC cxx_std_20)

# -----------------------------------------------------------------------------
# hello_app 可执行文件
#   - 链接 greeter 用 PRIVATE：exe 不会被别的 target 再链接
#   - greeter 的 PUBLIC include 会自动传过来，所以 main.cpp 能
#     #include <greeter/greeter.hpp> 而不用这里再写 include_directories
# -----------------------------------------------------------------------------
add_executable(hello_app
    app/main.cpp
)

target_link_libraries(hello_app PRIVATE greeter)
