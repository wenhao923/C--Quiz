# C--Quiz

基于 C++ 的纯原生开发实践项目，聚焦 C++ 核心特性、高级组件封装、多线程编程与跨端图形化开发，包含核心组件、编程练习及 Boids 鱼群模拟实战项目，用于 C++ 进阶学习与工程实践。

### 技术栈
- **主语言**：C++
- **图形渲染**：SFML
- **图形界面**：ImGui
- **编译适配**：VSCode跨端编译配置、Clang/MSVC
- **开发环境**：MacOS, Windows

### 项目结构
| 目录 | 核心内容 |
|------|----------|
| **core** | 通用组件封装|
| **projects/Boids** | Boids鱼群模拟 |
| **exercise** | C++核心特性编程练习案例 |
| **thirdParty** | SFML、ImGui等第三方依赖库 |
| **.vscode** | 跨平台编译与调试配置 |

### 功能
- 集成SFML图形渲染框架、ImGui图形界面
- 验证MacOS, Windows 双平台编译运行
- 实现自定义 **MyVector** : Rule of 5，push_back(), emplace_back(), 迭代器
- 实现线程池 **MyThreadPool** ：任务队列, 互斥锁, 条件变量, 原子变量

### MileStone
- 2000个Boid场景下，线程池将帧率从40FPS提升到120FPS

### Todo 
- [ ] 内存池
- [ ] Unordered_Map
- [ ] String
- [ ] Vulkan