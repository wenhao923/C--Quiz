# 收集 Boids 游戏的所有源码
set(GAME_SOURCES
    main.cpp
    # BoidsSystem.cpp 等具体的游戏逻辑
)

# 生成最终的 Boids.exe
add_executable(Triangle ${GAME_SOURCES})

# [架构级解耦] Boids.exe 只需要知道 MyEngineCore 的存在！
# 它根本不需要去操心什么 Dawn、SFML，它只管调用 Engine::Get().Init()
target_link_libraries(Triangle PRIVATE MyEngineCore)

#set_target_properties(Triangle PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

if (EMSCRIPTEN)
    set_target_properties(Triangle PROPERTIES SUFFIX ".html")
endif()