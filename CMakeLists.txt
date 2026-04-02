# ==============================================================================
# MyEngine
# 物理路径: MyEngine/CMakeLists.txt
# ==============================================================================

# 1. 启动全局引擎
cmake_minimum_required(VERSION 3.15)
project(MyEngine VERSION 1.0.0 LANGUAGES CXX C)

# 强制全局使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    set(PLATFORM_NAME "Windows")
elseif(APPLE)
    set(PLATFORM_NAME "macOS")
else()
    set(PLATFORM_NAME "Linux")
endif()

# ==============================================================================
# 2. 统一输出目录
# 将所有的 .exe, .dll, .lib 统一输出到根目录的 bin 和 lib 文件夹下
# ==============================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${PLATFORM_NAME}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/${PLATFORM_NAME}/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/${PLATFORM_NAME}/$<CONFIG>")

message(STATUS "📁 Output Directory: bin/${PLATFORM_NAME}/$<CONFIG>")

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