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

target_include_directories(MyEngineCore PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if (EMSCRIPTEN)
    # [Emscripten 标准] 使用官方 emdawnwebgpu 端口
    # 这个端口提供 webgpu 头文件和 JavaScript 绑定，可以直接调用浏览器的原生 WebGPU API
    # 💡 不需要链接任何库，浏览器会直接提供 WebGPU 运行时支持
    
    target_compile_options(MyEngineCore PUBLIC 
        "--use-port=emdawnwebgpu"
        "-sUSE_GLFW=3"
    )
    
    target_link_options(MyEngineCore PUBLIC 
        "--use-port=emdawnwebgpu"
        "-sUSE_GLFW=3"
        "-sWASM=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sASYNCIFY=1"
    )
    
    # Emscripten ports 会将头文件放在 cache 目录，需要明确指定搜索路径
    # 从 Emscripten 环境变量获取 SDK 路径
    if(DEFINED ENV{EMSCRIPTEN})
        set(EMSCRIPTEN_SDK_PATH $ENV{EMSCRIPTEN})
    else()
        # 备选：如果没有环境变量，尝试从 toolchain 推导
        set(EMSCRIPTEN_SDK_PATH ${EMSCRIPTEN})
    endif()
    
    # 添加 WebGPU 头文件搜索路径
    target_include_directories(MyEngineCore PUBLIC
        "${EMSCRIPTEN_SDK_PATH}/cache/wasm/webgpu"
    )
else()
    # 桌面端：需要链接完整的 Dawn 库和 GLFW
    target_link_libraries(MyEngineCore 
        PUBLIC
            webgpu_dawn
            dawn_glfw
            glfw
    )
endif()