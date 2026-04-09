# ==============================================================================
# MyEngine
# 物理路径: MyEngine/CMakeLists.txt
# ==============================================================================

# 1. 启动全局引擎
cmake_minimum_required(VERSION 3.19)
project(MyEngine VERSION 1.0.0 LANGUAGES CXX C)

# 强制全局使用 C++17 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF) # 保证跨平台纯洁性


# 2. 路由架构，设置输出目录
if(EMSCRIPTEN)
    set(TARGET_ARCH "WebAssembly")
elseif(MSVC)
    set(TARGET_ARCH "Windows")
elseif(APPLE)
    set(TARGET_ARCH "macOS")
endif()
message(STATUS "🎯 正在为架构 [ ${TARGET_ARCH} ] 配置输出路由...")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ==============================================================================
# 3. 挂载外部库 (Third-Party)
# ==============================================================================

# 挂载 Dawn (WebGPU底层)
if (NOT EMSCRIPTEN)
    set(TINT_BUILD_IR_BINARY OFF CACHE BOOL "" FORCE)
    set(DAWN_BUILD_PROTOBUF OFF CACHE BOOL "" FORCE)
    set(DAWN_BUILD_SAMPLES OFF CACHE BOOL "Don't build Dawn samples" FORCE)
    set(DAWN_ENABLE_TESTS OFF CACHE BOOL "Don't build Dawn tests" FORCE)
    add_subdirectory(thirdParty/dawn)
endif()

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