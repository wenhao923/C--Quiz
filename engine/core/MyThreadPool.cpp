#include "MyThreadPool.h"

MyThreadPool::MyThreadPool()
{
    int cores = std::thread::hardware_concurrency();
    for (size_t i = 0; i < cores; i++)
    {
        workers.emplace_back([this](){
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);

                    this->job_condition.wait(lock, [this]() {
                        return this->stop || !this->tasks.empty();
                    });

                    if (this->stop && this->tasks.empty())
                    {
                        return;
                    }

                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();

                this->taskRuningCount--;
                if (this->taskRuningCount == 0)
                {
                    {
                        std::lock_guard<std::mutex> lock(wait_mutex);
                    }
                    this->tasks_complete.notify_one();
                }
            }      
        });
    }
}

MyThreadPool::~MyThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        this->stop = true;
    }

    this->job_condition.notify_all();

    for (std::thread &worker : this->workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}

void MyThreadPool::enqueue_simple(std::function<void()> task) 
{
    {
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        
        if (this->stop) 
            return;
        
        this->tasks.push(task);
    }

    this->taskRuningCount++;

    this->job_condition.notify_one();
}

void MyThreadPool::wait_all()
{
    std::unique_lock<std::mutex> lock(this->wait_mutex);
    this->tasks_complete.wait(lock, [this]() {
                        return this->taskRuningCount == 0;
                    });
}
