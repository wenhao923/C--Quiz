#include <iostream>
#include <chrono>
#include <mutex>
#include "MyThreadPool.h"

std::mutex cout_mutex; // 用于保护控制台输出，防止多线程打印字符交错

// 模拟一个需要执行的耗时任务
void simulate_work(int task_id, std::atomic<int>& counter) {
    // 模拟耗时操作 (例如 I/O 或者复杂计算)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 安全地打印输出，验证多个线程在同时工作
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "任务 " << task_id 
                  << " 正在执行，工作线程 ID: " << std::this_thread::get_id() << std::endl;
    }

    // 记录完成的任务数（原子操作，保证线程安全）
    counter++;
}

int main() {
    std::cout << "--- 开始测试 MyThreadPool 和 MyVector ---" << std::endl;

    std::atomic<int> completed_tasks(0);
    const int total_tasks = 50; // 总共提交 50 个任务

    // 记录开始时间
    auto start_time = std::chrono::high_resolution_clock::now();

    {
        // 创建线程池，自动根据硬件核心数初始化 MyVector<std::thread>
        MyThreadPool pool; 
        
        std::cout << "主线程提交任务中..." << std::endl;
        for (int i = 0; i < total_tasks; ++i) {
            pool.enqueue_simple([i, &completed_tasks]() {
                simulate_work(i, completed_tasks);
            });
        }
        std::cout << "主线程提交任务完毕，等待线程池处理..." << std::endl;
        
        // 当 pool 离开此作用域时，将调用 ~MyThreadPool()
        // 你的析构函数逻辑会设置 stop = true，通知所有线程，并 join 它们
    }

    // 记录结束时间
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end_time - start_time;

    // 输出统计结果
    std::cout << "\n--- 测试结果 ---" << std::endl;
    std::cout << "预期完成任务数: " << total_tasks << std::endl;
    std::cout << "实际完成任务数: " << completed_tasks.load() << std::endl;
    std::cout << "总耗时: " << elapsed.count() << " ms" << std::endl;

    // 验证：如果是单线程串行执行，50 * 100ms = 5000ms。
    // 如果总耗时远小于 5000ms，说明线程池并发工作正常。
    if (completed_tasks == total_tasks) {
        std::cout << "✅ 成功：所有任务都已安全完成，没有丢失！" << std::endl;
    } else {
        std::cout << "❌ 失败：部分任务未完成或发生丢失。" << std::endl;
    }

    return 0;
}