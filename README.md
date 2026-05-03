# cpp20_4_hello

> 一个最小化的 **CMake + C++20** 工程骨架，作为 HFT / 合约做市系统学习之路的第 0 步。
> 聚焦在"把工程结构搞懂"，业务代码只是一行 `Hello, HFT world!`。

---

## 一、项目结构

```
cpp20_4_hello/
├── CMakeLists.txt              顶层构建脚本（唯一一份）
├── CMakePresets.json           项目级预设，提交 git
├── CMakeUserPresets.json       个人本地预设，gitignore 掉
├── README.md                   你正在读的这份文档
├── .gitignore
├── include/
│   └── greeter/
│       └── greeter.hpp         greeter 库的公共 API
├── src/
│   └── greeter.cpp             greeter 库的实现
└── app/
    └── main.cpp                hello_app 可执行文件入口
```

**一句话设计思想**：业务逻辑做成 **库**（`greeter`），可执行文件（`hello_app`）只是个薄壳。以后你写的 `order_book` / `matching_engine` / `feed_handler` 每一个模块都照这个模板复制一遍就行。

---

## 二、怎么构建和运行

环境要求：CMake 3.21+、支持 C++20 的编译器（g++ 13+ 或 clang++ 16+）。

### 2.1 推荐方式：用 CMakePresets（命令最短、以后实际开发会这么用）

```bash
# 第一步：看看有哪些可用预设
cmake --list-presets

# 第二步：配置（生成构建系统，产物进 build/debug/）
cmake --preset debug

# 第三步：构建
cmake --build --preset debug

# 第四步：运行
./build/debug/hello_app
```

你应该看到输出：

```
Hello, HFT world! (built with C++20)
```

想切 Release：把命令里的 `debug` 换成 `release`，产物会自动进 `build/release/`，不会覆盖 debug 版本。

### 2.2 传统方式（没有预设时就这么玩，本质跟预设完全等价）

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
./build/hello_app
```

两种方式产生的二进制文件完全一样。预设只是把 `-S . -B build/debug -DCMAKE_BUILD_TYPE=Debug` 这类长命令"固化"成 JSON 而已，没有任何魔法。

### 2.3 个人预设（演示 `CMakeUserPresets.json`）

仓库里留了一个示例，在 `debug` 基础上加了 `-Wall -Wextra -Wpedantic`：

```bash
cmake --preset dev
cmake --build --preset dev
./build/dev/hello_app
```

> 注意：`CMakeUserPresets.json` 已经在 `.gitignore` 里，这里只是保留做教学示例。真实项目中每个开发者自己建自己的那份，不入库。

---

## 三、逐行读懂 `CMakeLists.txt`（这是这份 demo 的主菜）

```cmake
cmake_minimum_required(VERSION 3.21)
```

声明"本脚本至少需要 CMake 3.21"。3.21 是支持 **Presets v3** 的最低版本。必须写在第一行。

```cmake
project(cpp20_4_hello
    VERSION 0.1.0
    LANGUAGES CXX
)
```

声明项目名、版本、使用的语言。`CXX` 就是 C++。`project()` 还会自动探测编译器、填一堆 `PROJECT_*` 变量，必须放在 `cmake_minimum_required` 之后、任何 target 之前。

```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

三件套，含义：

| 变量 | 作用 |
|---|---|
| `CMAKE_CXX_STANDARD 20` | 之后创建的 target 默认用 C++20 |
| `CMAKE_CXX_STANDARD_REQUIRED ON` | 编译器不支持 C++20 时**报错退出**，而不是悄悄降级到 C++17 |
| `CMAKE_CXX_EXTENSIONS OFF` | 关掉 GNU 扩展（`-std=c++20` 而不是 `-std=gnu++20`），坚持标准 C++ |

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

让 CMake 在 build 目录生成 `compile_commands.json`。这个文件列出"每个源文件用了哪些编译参数"，**clangd**（Cursor/VSCode 的 C++ 语言服务后端）靠它做跳转、补全、错误提示。没它的话编辑器就瞎了。

```cmake
add_library(greeter STATIC
    src/greeter.cpp
)
```

定义一个名叫 `greeter` 的**静态库** target，由 `src/greeter.cpp` 编译而成。`STATIC` 表示静态库（`.a` 文件），另一个常见选项是 `SHARED`（动态库，`.so` 文件）。HFT 工程里绝大多数内部库都用 `STATIC` —— 部署简单、链接后没有 dlopen 开销。

```cmake
target_include_directories(greeter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

告诉 `greeter`：你的头文件在 `${CMAKE_CURRENT_SOURCE_DIR}/include`（即项目根下的 `include/`）。**关键词是 `PUBLIC`**：

- `PRIVATE`：只有 `greeter` 自己的源文件能 `#include`，链接它的 target 看不到
- `PUBLIC`：`greeter` 自己要用 + **链接它的 target 也能用**（最常见）
- `INTERFACE`：`greeter` 自己不用，但要求链接它的 target 必须用（做纯头文件库时用）

所以标成 `PUBLIC` 之后，后面 `hello_app` 链接 `greeter` 时**不用再写一遍 include**，自动就能 `#include <greeter/greeter.hpp>`。

```cmake
target_compile_features(greeter PUBLIC cxx_std_20)
```

显式要求 `greeter` 使用 C++20 特性。这行跟前面的 `set(CMAKE_CXX_STANDARD 20)` 功能有重叠，**但作用层级不同**：

- 全局的 `CMAKE_CXX_STANDARD`：默认值，**新加 target 忘了写也兜得住**
- target 级的 `target_compile_features`：**显式声明**，还会传递给链接此 target 的其他 target（因为用了 `PUBLIC`）

两者一起写是业界常见的"防御式"风格，不矛盾。以后项目复杂了、有些 target 想用 C++23、有些用 C++20 混跑时，只改 target 级设置就够了。

```cmake
add_executable(hello_app
    app/main.cpp
)
```

定义一个叫 `hello_app` 的**可执行文件** target，由 `app/main.cpp` 编译。

```cmake
target_link_libraries(hello_app PRIVATE greeter)
```

让 `hello_app` 链接 `greeter`。`PRIVATE` 表示"只有我自己用"——因为可执行文件不会再被别人链接，`PRIVATE` 最干净。

这一行干了两件事：
1. 在链接阶段把 `greeter.a` 链进 `hello_app`
2. 把 `greeter` 的 `PUBLIC` 属性（include 路径、编译特性）传递给 `hello_app`

所以 `main.cpp` 里的 `#include <greeter/greeter.hpp>` 能解析到，就是靠第 2 点。

---

## 四、逐字段读懂 `CMakePresets.json`

```json
{
    "version": 3,
    "cmakeMinimumRequired": { "major": 3, "minor": 21, "patch": 0 },
```

预设文件格式自己也有版本号。`"version": 3` 对应 CMake 3.21+。

```json
    "configurePresets": [
        {
            "name": "base",
            "hidden": true,
            "generator": "Unix Makefiles",
            "binaryDir": "${sourceDir}/build/${presetName}"
        },
```

- `"hidden": true` —— 这个预设只给别人"继承"用，自己不能直接被 `--preset base` 调用
- `"generator": "Unix Makefiles"` —— 在 Linux/WSL 上最通用。可选 `Ninja`（更快，但要额外装）
- `"binaryDir": "${sourceDir}/build/${presetName}"` —— 关键点：用 `${presetName}` 让不同预设自动用不同 build 目录，Debug 和 Release 产物永远不打架

```json
        {
            "name": "debug",
            "inherits": "base",
            "cacheVariables": { "CMAKE_BUILD_TYPE": "Debug" }
        },
```

- `"inherits": "base"` —— 继承 base 的 generator 和 binaryDir
- `"cacheVariables"` —— 设置 CMake cache 变量，等价于命令行 `-DCMAKE_BUILD_TYPE=Debug`

`release` 预设同理，把 `Debug` 换成 `Release` 即可。

```json
    "buildPresets": [
        { "name": "debug",   "configurePreset": "debug"   },
        { "name": "release", "configurePreset": "release" }
    ]
```

`buildPresets` 配对 `configurePresets`，让 `cmake --build --preset debug` 能直接找到对应的 build 目录。

---

## 五、`CMakePresets.json` vs `CMakeUserPresets.json`

| | `CMakePresets.json` | `CMakeUserPresets.json` |
|---|---|---|
| 谁的 | 项目的 | 个人的 |
| 提交 git 吗 | 提交 | **不**提交（`.gitignore`） |
| 放什么 | 所有人都该共享的配置（C++ 标准、build dir 布局、必须的 cache 变量） | 你自己想要但不想强加给别人的（严格警告、自定义编译器路径、asan/ubsan 开关、PGO 参数…） |
| 能互相继承吗 | 不能继承 user 的 | **可以**继承项目的（本仓库的 `dev` preset 就继承自 `debug`） |

从第一天起划清这条线，以后你开始给别人 review / 被 review 时会感激现在的自己。

---

## 六、试试下面这些练习

> 每个练习都在 10 分钟内可完成，强烈建议**亲手跑一遍**，不要只是看。

1. **看配置输出**：跑 `cmake --preset debug`，把输出从头读一遍。重点看：
   - 检测到的编译器是谁、版本多少
   - `Build type: Debug` 这一行
   - 生成的 `build/debug/` 里有哪些文件（有一个 `Makefile`、有一个 `compile_commands.json`）

2. **看构建输出**：跑 `cmake --build --preset debug --verbose`（加 `--verbose`），观察真实的 `g++` 命令长啥样。你会看到 `-std=c++20`、`-I.../include`、`-g` 这些参数是怎么被 CMake 组织出来的。

3. **增量编译观察**：改一下 `src/greeter.cpp` 里的字符串，再跑一次 `cmake --build --preset debug`。**只有 `greeter.cpp` 和链接阶段会重跑**，`main.cpp` 不重编。再改一次 `include/greeter/greeter.hpp`，这次 `main.cpp` 也会重编——因为它 include 了这个头。

4. **Debug vs Release 对比**：
   ```bash
   cmake --preset release && cmake --build --preset release
   ls -l build/debug/hello_app build/release/hello_app
   ```
   Release 版本通常更小（去掉了调试符号），运行速度也更快（`-O3`）。

5. **故意制造一个编译错误**：在 `main.cpp` 里写一句错的代码（比如 `int x = "hello";`），`cmake --build --preset debug` 看错误信息；再换到 `dev` preset（带 `-Wpedantic`）看看错误信息和警告有没有不同。

6. **对着 [四、逐字段读懂 CMakePresets.json](#四逐字段读懂-cmakepresetsjson) 自己默写一遍 preset 文件**，不看答案能写出来就说明吃透了。

---

## 七、这份 demo 刻意不做的事

为了聚焦在工程骨架上，下面这些**没有**加进来。每一条都标注了"什么时候该加"，免得你过早焦虑：

| 功能 | 触发添加的信号 |
|---|---|
| 单元测试（GoogleTest） | 你写完第一个有逻辑的函数，开始问"我咋知道它对不对" |
| 日志库（spdlog） | 开始处理行情/订单流，`std::cout` 不够用 |
| 外部依赖管理（vcpkg / Conan / FetchContent） | 第一次需要 Boost.Asio、cppzmq 等第三方库 |
| 嵌套 CMakeLists（`add_subdirectory`） | library target 超过 3 个、顶层 CMakeLists 超过一屏 |
| 严格警告 + `-Werror` | 习惯了警告的样子，想开始真的不容忍它们 |
| Sanitizers（asan/ubsan/tsan） | 写完订单簿，开始怀疑越界/UB/数据竞争 |

---

## 八、下一步学什么

这份 demo 的下一个合理进化方向（由你选，一次只加一样）：

1. **加一个单元测试模块**：`tests/greeter_test.cpp`，用 `FetchContent` 拉 GoogleTest
2. **把 greeter 换成真正有意义的东西**：比如 `tick_buffer`（环形缓冲区），为后面接行情打基础
3. **加第二个库**：比如 `utils`（时间戳、统计），让 `hello_app` 同时链接 `greeter` + `utils`，感受多模块依赖
4. **加一个 sanitizer preset**：`CMakeUserPresets.json` 里加 `asan` preset，`inherits: "debug"` + `CMAKE_CXX_FLAGS: "-fsanitize=address,undefined -fno-omit-frame-pointer"`

任何一条你想走，告诉我，我带你一步一步加。
