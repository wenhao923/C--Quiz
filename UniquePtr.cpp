template<typename T>
class MyUniquePtr {
public:
    MyUniquePtr(T* ptr = nullptr): rawPtr(ptr) {}
    ~MyUniquePtr() { 
        if (rawPtr)
        {
            delete rawPtr;
            rawPtr = nullptr;
        }
    }

    MyUniquePtr(const MyUniquePtr&) = delete;
    MyUniquePtr(MyUniquePtr&& other) noexcept {
        rawPtr = other.rawPtr;
        other.rawPtr = nullptr;
    }

    MyUniquePtr& operator=(const MyUniquePtr&) = delete;
    MyUniquePtr& operator=(MyUniquePtr&& other) noexcept {
        if (this != &other)
        {
            rawPtr = other.rawPtr;
            other.rawPtr = nullptr;
        }
        return *this;
    }

    T* get() const noexcept { return rawPtr; }
    T* operator->() const noexcept { return rawPtr; }
    T& operator*() const noexcept { return *rawPtr; }
private:
    T* rawPtr = nullptr;
};

总评分：60/100。

1. 致命错误：移动赋值时的内存泄漏 (Memory Leak)
请看这段代码：

C++

MyUniquePtr& operator=(MyUniquePtr&& other) noexcept {
    if (this != &other)
    {
        // ❌ 致命错误在这里！
        // 如果 this->rawPtr 原本指向了一块内存（比如对象 A），
        // 你直接把 other.rawPtr (对象 B) 覆盖了它。
        // 结果：对象 A 变成了孤魂野鬼，没有人 delete 它，内存泄漏！
        rawPtr = other.rawPtr; 
        
        other.rawPtr = nullptr;
    }
    return *this;
}
正确逻辑： 在你接管新资源之前，你必须先清理掉手里的旧资源（如果存在的话）。

2. 小建议：析构函数的冗余检查
C++

~MyUniquePtr() { 
    if (rawPtr) // <--- 这个 if 是多余的
    {
        delete rawPtr;
        rawPtr = nullptr; // <--- 析构都要结束了，置空也没意义，会被优化掉
    }
}
在 C++ 中，delete nullptr 是绝对安全且合法的操作（什么都不会发生）。所以你可以直接写：

C++

~MyUniquePtr() { 
    delete rawPtr;
}

3. 安全性建议：explicit 关键字
构造函数建议加 explicit，防止隐式转换带来的意外所有权接管。

C++

// 建议
explicit MyUniquePtr(T* ptr = nullptr) : rawPtr(ptr) {}

// 如果不加 explicit，下面这种危险代码是合法的：
// MyUniquePtr<int> p = new int(10); 
// 这种隐式转换在现代 C++ 中通常被认为是不够安全的（虽然方便）。