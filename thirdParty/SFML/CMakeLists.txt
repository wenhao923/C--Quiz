# 物理路径：thirdParty/SFML/CMakeLists.txt

# 1. 嗅探当前操作系统，指向正确的预编译目录
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(SFML_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/windows-x64")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin") # MacOS
    set(SFML_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/macos-arm64")
else()
    message(FATAL_ERROR "Unsupported platform for precompiled SFML!")
endif()

# 2. 核心魔法：创建一个“空”的接口库 (INTERFACE)
# 它自己不编译任何东西，只负责当一个快递员，把路径传递给使用它的人
add_library(SFML_Bundle INTERFACE)

# 3. 告诉别人：SFML 的头文件在哪，库文件在哪
target_include_directories(SFML_Bundle INTERFACE "${SFML_ROOT}/include")
target_link_directories(SFML_Bundle INTERFACE "${SFML_ROOT}/lib")

# 4. 告诉别人：需要链接哪些具体的库文件 (基础三大件)
target_link_libraries(SFML_Bundle INTERFACE 
    sfml-graphics 
    sfml-window 
    sfml-system
)