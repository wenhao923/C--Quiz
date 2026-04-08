#include <core/Engine.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    // 1. 初始化 GLFW 引擎
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // 2. [架构师核心护盾] 警告 GLFW：不要擅自创建 OpenGL 上下文！
    // 因为我们要自己用 WebGPU 接管窗口，必须加这一行，否则会冲突报错
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    // (可选) 禁止窗口缩放，避免早期处理 resize 逻辑
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // 3. 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "MyNextGenEngine - GLFW", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // 初始化我们的次世代引擎
    Engine::Get().Init(window); // 我们要把窗口句柄传进去

    while (!glfwWindowShouldClose(window)) {
        // 处理操作系统的事件 (鼠标、键盘、关闭按钮)
        glfwPollEvents();

        // 呼叫 WebGPU 绘制下一帧
        Engine::Get().Render();
    }

    // 6. 优雅地清理现场
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}