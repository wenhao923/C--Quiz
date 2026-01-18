template<typename T>
class MySharedPtr {
public:
    MySharedPtr(T* ptr = nullptr) : rawPtr(ptr) noexcept {
        if (ptr)
        {
            b = new Block();
            b->refCount = 1;
        } else {
            b = nullptr;
        }
    }

    ~MySharedPtr() noexcept {
        release();
    }

    MySharedPtr(const MySharedPtr& other) noexcept {
        copyFrom(other);
    }

    MySharedPtr(MySharedPtr&& other) noexcept {
        moveFrom(other);
    }

    MySharedPtr& operator=(const MySharedPtr& other) {
        if (this != &other)
        {
            release();
            copyFrom(other);
        }
        return *this;
    }

    MySharedPtr& operator=(MySharedPtr&& other) {
        if (this != &other)
        {
            release();
            moveFrom(other);
        }
        return *this;
    }

    T* get() const noexcept { return rawPtr; }
    T* operator->() const noexcept { return rawPtr; }
    T& operator*() const noexcept { return *rawPtr; }

private:
    T* rawPtr = nullptr;
    struct Block {
        unsigned refCount = 0;
    }
    Block* b = nullptr;

    void release() {
        if (b)
        {
            b->refCount--;
            if (b->refCount == 0)
            {
                delete rawPtr;
                delete b;
            }
        } 
        rawPtr = nullptr;
        b = nullptr;
    }

    void copyFrom(const MySharedPtr& other) {
        rawPtr = other.rawPtr;
        b = other.b;

        if (b)
        {
            b->refCount++;
        }
    }

    void moveFrom(MySharedPtr& other) {
        rawPtr = other.rawPtr;
        b = other.b;

        other.rawPtr = nullptr;
        otehr.b = nullptr;
    }
};


这份代码的逻辑已经非常完善了！评分可以给到 90/100。

一、 两个必修的编译错误 (Syntax Errors)
这两个错误会导致代码无法通过编译：

拼写错误： 在 moveFrom 函数中：

C++

otehr.b = nullptr; // ❌ 拼写错误：otehr -> other
结构体定义缺失分号： 在 private 区域定义 struct Block 时：

C++

struct Block {
    unsigned refCount = 0;
} // ❌ 缺少分号 ;
Block* b = nullptr;
修正：应该为 };。

二、 一个隐藏的“诚实”问题：noexcept
看你的构造函数：

C++

MySharedPtr(T* ptr = nullptr) : rawPtr(ptr) noexcept { // <--- 标记了 noexcept
    if (ptr) {
        b = new Block(); // <--- 这里可能会抛出 std::bad_alloc
        // ...
    }
}
问题：new 操作符在内存不足时会抛出 std::bad_alloc 异常。

后果：你标记了 noexcept，意味着你向编译器承诺“我绝不抛异常”。如果 new 真的抛了异常，C++ 运行时会直接调用 std::terminate() 强行杀死程序，导致无法被外部的 try-catch 捕获。

建议：去掉构造函数的 noexcept。除非你使用 new (std::nothrow) Block() 并处理空指针。