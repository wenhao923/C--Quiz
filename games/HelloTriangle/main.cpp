#include <core/Engine.h>
#include <GLFW/glfw3.h>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

void MainLoopStep() {
    glfwPollEvents();
    Engine::Get().Render();
}

GLFWwindow* g_window = nullptr;

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    g_window = glfwCreateWindow(800, 600, "MyNextGenEngine - GLFW", nullptr, nullptr);
    if (!g_window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    Engine::Get().InitAsync(g_window, []() {
        std::cout << "[Main] Engine 信号确认！正式点火启动主循环！" << std::endl;
        
        #ifdef __EMSCRIPTEN__
            emscripten_set_main_loop(MainLoopStep, 0, true);
        #else
            while (!glfwWindowShouldClose(g_window)) {
                MainLoopStep();
            }
        #endif
    });

#ifndef __EMSCRIPTEN__
    glfwDestroyWindow(g_window);
    glfwTerminate();
#endif

    return 0;
}