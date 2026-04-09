# C++ Quiz

基于 C++ 的纯原生开发实践项目，聚焦 C++ 核心特性、高级组件封装、多线程编程与跨端图形化开发，包含自定义引擎核心组件、编程练习及 Boids 鱼群模拟实战项目，用于 C++ 进阶学习与工程实践。

## 技术栈
- **主语言**：C++
- **编译系统**：CMake
- **渲染框架**：WebGPU (Dawn)、SFML
- **图形界面**：ImGui、ImGui-SFML
- **编译器**：MSVC (Visual Studio)、Emscripten (WebGPU)
- **开发环境**：Windows, macOS, Web

## 项目结构

```
.
├── engine/              # 核心引擎组件
│   └── core/           # 引擎核心类与自定义数据结构
├── games/              # 游戏项目集合
│   ├── HelloTriangle/  # WebGPU 三角形渲染 (Emscripten 支持)
│   └── Boids/          # Boids 鱼群模拟实战项目 (SFML 渲染库)
├── playground/         # C++ 编程练习案例
├── thirdParty/         # 第三方依赖库
│   ├── dawn/          # WebGPU 渲染框架
│   ├── imgui/         # GUI 库
│   ├── imgui-sfml/    # ImGui-SFML 集成
│   └── SFML/          # 图形渲染库
├── build/             # 编译输出目录
│   ├── build-win/     # Windows 构建
│   │   └── bin/      # MSVC 编译产物 (exe)
│   └── build-web/     # Emscripten Web 构建
│       └── bin/      # Web 编译产物 (HTML/WebAssembly)
└── CMakeLists.txt     # CMake 项目配置
```

## 核心组件

| 组件 | 功能描述 | 关键特性 |
|------|----------|---------|
| **Engine** | 图形引擎核心 | 异步任务、事件循环 |
| **MyVector** | 动态数组 | Rule of 5、迭代器、emplace_back |
| **MyThreadPool** | 线程池 | 任务队列、同步原语、工作窃取 |
| **MyUnorderedMap** | 哈希表 | 增删查、Lazy初始化、Open Addressing |
| **WebGPUAsync** | 异步渲染 | GPU 任务队列、异步交换链 |

## 构建与运行

### Windows 构建
```
1. git submodule update --init
2. python tools/fetch_dawn_dependencies.py
3. 使用CMake构建工程
4. 使用MSVC编译工程
5. 拷贝图形API动态库到exe目录
```

### Web 构建 (Emscripten)
```
1. emsdk缓存经常出错，找不到需要的头文件：需要embuilder build sysroot --force
2. 对于html需要 open with live server
```

## Milestone
- 🎯 **自定义容器库**：从零实现 STL 风格的容器和工具类
- 🎯 **线程池**：多线程优化2000个Boid，帧率提升 3 倍
- 🎯 **WebGPU 跨端渲染**：支持 Web/Windows/macOS 统一渲染管道
## 计划中 (TODO)
- [ ] 内存池 (Memory Pool)
- [ ] String 类完整实现
- [ ] 深入了解现代图形API
- [ ] 3D 模型加载 (Assimp / cgltf)
- [ ] 延迟渲染 (Deferred Rendering)
- [ ] 物理引擎集成

## 更新日志

### 4月
- 接入dawn，窗口API替换为GLFW，在Windows渲染出HelloTriangle
- 使用emscripten编译出产物，在chrome渲染出HelloTriangle

### 3月
- 完成 MyVector、MyThreadPool、MyUnorderedMap 核心实现
- Boids 鱼群模拟基础版本