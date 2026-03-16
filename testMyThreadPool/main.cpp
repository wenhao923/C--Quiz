#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#include "imgui.h"
#include "imgui-SFML.h"

// --- 全局常量设置 ---
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const int NUM_BOIDS = 2000;          // 单线程下 2000 条已经是极限测试了

// ==========================================
// [黑魔法区] 可以被 UI 实时修改的全局参数！
// ==========================================
float neighbor_radius = 50.0f; 
float max_speed = 4.0f;        
float weight_separation = 1.5f; 
float weight_alignment = 0.05f; 
float weight_cohesion = 0.005f;

// --- 鱼的结构体 ---
struct Boid {
    sf::Vector2f position;
    sf::Vector2f velocity;
};

// 双缓冲状态数组 (即便单线程也保留，防止计算先后顺序导致的误差)
std::vector<Boid> boids_read(NUM_BOIDS);
std::vector<Boid> boids_write(NUM_BOIDS);

// --- 辅助数学函数 ---
float get_length(const sf::Vector2f& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}
sf::Vector2f normalize(const sf::Vector2f& v) {
    float len = get_length(v);
    return len == 0.0f ? sf::Vector2f(0.0f, 0.0f) : sf::Vector2f(v.x / len, v.y / len);
}
float get_distance(const sf::Vector2f& a, const sf::Vector2f& b) {
    return get_length(sf::Vector2f(a.x - b.x, a.y - b.y));
}

// --- 核心逻辑：计算单条鱼的下一帧状态 ---
void update_single_boid(int index) {
    const Boid& me = boids_read[index];
    sf::Vector2f separation(0.0f, 0.0f);
    sf::Vector2f alignment(0.0f, 0.0f);
    sf::Vector2f cohesion(0.0f, 0.0f);
    int total_neighbors = 0;

    // 暴力遍历寻找邻居
    for (int i = 0; i < NUM_BOIDS; ++i) {
        if (i == index) continue;
        const Boid& other = boids_read[i];
        
        float dist = get_distance(me.position, other.position);

        if (dist > 0 && dist < neighbor_radius) {
            // 1. 分离 (越近斥力越大)
            sf::Vector2f diff = me.position - other.position;
            separation += sf::Vector2f(diff.x / dist, diff.y / dist);
            // 2. 对齐 (累加邻居速度)
            alignment += other.velocity;
            // 3. 凝聚 (累加邻居位置)
            cohesion += other.position;
            total_neighbors++;
        }
    }

    sf::Vector2f new_velocity = me.velocity;
    if (total_neighbors > 0) {
        alignment = sf::Vector2f(alignment.x / total_neighbors, alignment.y / total_neighbors);
        cohesion = sf::Vector2f(cohesion.x / total_neighbors, cohesion.y / total_neighbors);
        
        alignment -= me.velocity;
        cohesion -= me.position;
        
        // 使用动态权重
        new_velocity += separation * weight_separation + alignment * weight_alignment + cohesion * weight_cohesion;
    }

    // 限制最大速度
    float speed = get_length(new_velocity);
    if (speed > max_speed) {
        new_velocity = normalize(new_velocity) * max_speed;
    }

    // 计算新位置
    sf::Vector2f new_position = me.position + new_velocity;

    // 屏幕边缘环绕
    if (new_position.x < 0.0f) new_position.x += WINDOW_WIDTH;
    if (new_position.x >= WINDOW_WIDTH) new_position.x -= WINDOW_WIDTH;
    if (new_position.y < 0.0f) new_position.y += WINDOW_HEIGHT;
    if (new_position.y >= WINDOW_HEIGHT) new_position.y -= WINDOW_HEIGHT;

    // 写入 write 缓冲区
    boids_write[index].velocity = new_velocity;
    boids_write[index].position = new_position;
}


int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "单线程 Boids 鱼群基准测试");
    window.setFramerateLimit(120);

    // 1. 初始化 ImGui-SFML
    if (!ImGui::SFML::Init(window)) {
        std::cerr << "ImGui 初始化失败！" << std::endl;
        return -1;
    }

    // 初始化随机鱼群
    srand((unsigned int)time(NULL));
    for (int i = 0; i < NUM_BOIDS; ++i) {
        boids_read[i].position = sf::Vector2f(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT);
        float vx = (rand() % 10 - 5) / 5.0f;
        float vy = (rand() % 10 - 5) / 5.0f;
        boids_read[i].velocity = normalize(sf::Vector2f(vx, vy)) * max_speed;
    }

    sf::VertexArray flockMesh(sf::Triangles, NUM_BOIDS * 3);
    sf::Clock deltaClock; // 用于 ImGui 跟踪时间

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed) window.close();
        }

        // 3. 更新 ImGui 状态
        ImGui::SFML::Update(window, deltaClock.restart());

// ==========================================
        // [ImGui 面板绘制 - 纯英文版]
        // ==========================================
        ImGui::Begin("Boids Swarm Controller"); 
        
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Separator();
        
        ImGui::Text("Entity Count: %d", NUM_BOIDS);
        ImGui::SliderFloat("Max Speed", &max_speed, 1.0f, 15.0f);
        ImGui::SliderFloat("Vision Radius", &neighbor_radius, 10.0f, 150.0f);
        
        ImGui::Separator();
        ImGui::Text("Core Behavior Weights");
        
        // 分离：避免拥挤
        ImGui::SliderFloat("Separation", &weight_separation, 0.0f, 5.0f);
        // 对齐：方向一致
        ImGui::SliderFloat("Alignment", &weight_alignment, 0.0f, 0.5f);
        // 凝聚：向中心靠拢
        ImGui::SliderFloat("Cohesion", &weight_cohesion, 0.0f, 0.05f);
        
        ImGui::End();

        // ==========================================
        // [逻辑更新区] - 单线程纯 for 循环顺序计算
        // ==========================================
        for (int i = 0; i < NUM_BOIDS; ++i) {
            update_single_boid(i);
        }

        // 帧结束：读写缓冲区交换
        std::swap(boids_read, boids_write);

        // ==========================================
        // [渲染构建区]
        // ==========================================
        for (int i = 0; i < NUM_BOIDS; ++i) {
            const Boid& b = boids_read[i];
            
            sf::Vector2f forward = normalize(b.velocity);
            sf::Vector2f right(-forward.y, forward.x);
            float size = 5.0f;

            flockMesh[i * 3 + 0].position = b.position + forward * size;
            flockMesh[i * 3 + 0].color = sf::Color::Cyan;

            flockMesh[i * 3 + 1].position = b.position - forward * size - right * (size * 0.5f);
            flockMesh[i * 3 + 1].color = sf::Color::Blue;

            flockMesh[i * 3 + 2].position = b.position - forward * size + right * (size * 0.5f);
            flockMesh[i * 3 + 2].color = sf::Color::Blue;
        }

        // ==========================================
        // [GPU 绘制区]
        // ==========================================
        window.clear(sf::Color(15, 20, 30));
        window.draw(flockMesh);

        // 4. 最后一步：把 ImGui 画在最上层
        ImGui::SFML::Render(window);

        window.display();
    }

    // 5. 关闭前清理 ImGui 资源
    ImGui::SFML::Shutdown();
    return 0;
}