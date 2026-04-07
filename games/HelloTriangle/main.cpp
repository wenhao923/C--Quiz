#include <core/Engine.h>
#include <SFML/Window.hpp>

int main() {
    sf::Window window(sf::VideoMode(800, 600), "Dawn Triangle Attack");
    
    // 初始化我们的次世代引擎
    Engine::Get().Init(window); // 我们要把窗口句柄传进去

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        // 彻底抛弃 OpenGL 渲染，交给 Dawn
        // window.clear(); 删掉这行！
        // window.display(); 删掉这行！

        // 引擎，执行次世代渲染！
        Engine::Get().Render();
    }
    return 0;
}