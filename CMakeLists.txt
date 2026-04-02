# ==============================================================================
# MyEngine
# 物理路径: MyEngine/CMakeLists.txt
# ==============================================================================

# 1. 启动全局引擎
cmake_minimum_required(VERSION 3.15)
project(MyEngine VERSION 1.0.0 LANGUAGES CXX C)

# 1. 强制底层编译器（MSBuild/MSVC）输出纯英文日志，直接绕过中文 GBK 乱码危机
set(ENV{VSLANG} 1033)

# 2. 如果使用的是微软的 MSVC 编译器，强制它以 UTF-8 格式处理所有文件和输出
if(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    
    # 可选：消除一些 MSVC 独有的无聊警告，让日志更干净
    add_compile_options(/wd4819) # 消除“该文件包含不能在当前代码页中表示的字符”警告
endif()

# 强制全局使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==============================================================================
# 2. 统一输出目录
# 将所有的 .exe, .dll, .lib 统一输出到根目录的 bin 和 lib 文件夹下
# ==============================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

# ==============================================================================
# 3. 挂载外部库 (Third-Party)
# ==============================================================================

# 挂载 Dawn (WebGPU底层)
set(TINT_BUILD_IR_BINARY OFF CACHE BOOL "" FORCE)
set(DAWN_BUILD_PROTOBUF OFF CACHE BOOL "" FORCE)
set(DAWN_BUILD_SAMPLES OFF CACHE BOOL "Don't build Dawn samples" FORCE)
set(DAWN_ENABLE_TESTS OFF CACHE BOOL "Don't build Dawn tests" FORCE)
add_subdirectory(thirdParty/dawn)

add_subdirectory(thirdParty/SFML)
add_subdirectory(thirdParty/imgui-sfml)
add_subdirectory(thirdParty/imgui)

# ==============================================================================
# 4. 按依赖顺序启动内部流水线
# ==============================================================================

add_subdirectory(engine)
add_subdirectory(games)

# 第三步：(可选) 实验隔离区
# 如果你想编译平时刷的算法题，可以单独建一个 CMakeLists 然后在这里加进来
# add_subdirectory(playground)