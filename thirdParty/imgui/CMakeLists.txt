# 物理路径：thirdParty/imgui/CMakeLists.txt

# 1. 手动列出 ImGui 的核心源文件
set(IMGUI_SOURCES
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
)

# 2. 把它们编译成一个独立的静态库，就叫 ImGui_Static
add_library(ImGui_Static STATIC ${IMGUI_SOURCES})

# 3. 把它的头文件路径暴露出去 (PUBLIC)
# 这样你的引擎只要链接了 ImGui_Static，就能直接 #include "imgui.h"
target_include_directories(ImGui_Static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})