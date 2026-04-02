# 物理路径：thirdParty/imgui-sfml/CMakeLists.txt

# 1. 把 imgui-SFML.cpp 编译成静态库
add_library(ImGui_SFML STATIC imgui-SFML.cpp)

# 2. 暴露自己的头文件
target_include_directories(ImGui_SFML PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 3. 终极纽带：它依赖咱们刚才配置的 SFML_Bundle，以及 ImGui 核心
# (这里假设你在 thirdParty/imgui 下已经建好了 ImGui_Static)
target_link_libraries(ImGui_SFML PUBLIC 
    ImGui_Static 
    SFML_Bundle
)