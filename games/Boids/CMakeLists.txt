# ==============================================================================
# Boids 鸟群模拟游戏 - 业务可执行文件
# 物理路径: MyEngine/projects/Boids/CMakeLists.txt
# ==============================================================================

file(GLOB_RECURSE BOIDS_SOURCES 
    "*.cpp" 
    "*.h"
)

add_executable(Boids ${BOIDS_SOURCES})

target_link_libraries(Boids PRIVATE MyEngineCore)

set_target_properties(Boids PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")