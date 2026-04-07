# ==============================================================================
# MyEngine 核心静态库构建配置
# 物理路径: MyEngine/engine/CMakeLists.txt
# ==============================================================================

file(GLOB_RECURSE ENGINE_SOURCES 
    "core/*.cpp" 
    "core/*.h"
)

add_library(MyEngineCore STATIC ${ENGINE_SOURCES})

if(MSVC)
    target_compile_options(MyEngineCore PRIVATE /utf-8)
endif()

find_package(OpenGL REQUIRED)
target_link_libraries(MyEngineCore 
    PUBLIC
        webgpu_dawn    # 图形大炮
        SFML_Bundle    # 窗口与输入底盘 (必须显式加上！)
        ImGui_SFML     # UI 组件
        OpenGL::GL
)

target_include_directories(MyEngineCore PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)