#pragma once

// 尽量不在头文件里 include 太多东西，以加快整个项目的编译速度
struct GLFWwindow;

class Engine {
public:
    // 1. 获取引擎单例的全局唯一入口
    static Engine& Get() {
        static Engine instance;
        return instance;
    }

    // 2. 删掉拷贝构造和赋值函数，防止不小心复制出第二个引擎实例
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    // 3. 核心生命周期接口
    void Init(GLFWwindow* window);
    void Render();

private:
    // 4. 构造函数私有化，强迫外部只能通过 Engine::Get() 来访问
    Engine() = default;
    ~Engine() = default;

    /* 
     * 💡 架构师的隐藏设计：
     * 你会发现这里没有任何 wgpu::Device 或 wgpu::Queue 的成员变量。
     * 为什么？因为我们把它们写在了 Engine.cpp 的全局 static 变量里！
     * 这在 C++ 架构中是一种极端的“实现隐藏”：
     * 外部代码（比如 main.cpp）在 include "Engine.h" 时，根本不需要知道 WebGPU 的存在，
     * 也不需要引入庞大的 WebGPU 头文件。这极大地保护了模块的纯洁性，也极大缩短了编译时间。
     * （在更大型的引擎中，这种思想会演变成著名的 Pimpl 惯用法）
     */
};