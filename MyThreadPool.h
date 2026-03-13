#include "MyVector.h"
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <future>

class MyThreadPool
{
template<typename T>
using Vector = MyVector<T>;
public:
    MyThreadPool();
    ~MyThreadPool();

    void enqueue_simple(std::function<void()>);
private:
    Vector<std::thread> workers;

    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;

    bool stop = false;
};