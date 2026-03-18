template <typename T>
class MySharedPtr {
public:
    MySharedPtr(T* ptr = nullptr) : rawPtr(ptr) {
        if (ptr)
        {
            b = new Block();
            b->refCount = 1;
        }
        b = nullptr;
    }

    ~MySharedPtr() noexcept {
        release();
    }

    MySharedPtr(const MySharedPtr& other) noexcept {
        copyFrom(other);
    }

    MySharedPtr(MySharedPtr&& other) {
        moveFrom(other);
    }

    MySharedPtr& operator=(const MySharedPtr& other) noexcept {
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

    T* get() const noexcept {
        return rawPtr;
    }

    T* operator->() const noexcept {
        return rawPtr;
    }

    T& operator*() const noexcept {
        return *rawPtr;
    }
private:
    struct Block {
        unsigned refCount = 0;
    }
    T* rawPtr = nullptr;
    Block* b = nullptr;

    void release() noexcept {
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

    void copyFrom(const MySharedPtr& other) noexcept {
        rawPtr = other.rawPtr;
        b = other.b;

        if (b)
        {
            b->refCount++;
        }
        // 1. other可能是空么
        // 2. b可能是空么？要做保护么。对应的是刚用空指针构造出来，便拷贝构造。这个动作要直接报错么？
    }

    void moveFrom(MySharedPtr& other) {
        rawPtr = other.rawPtr;
        b = other.b;

        other.rawPtr = nullptr;
        other.b = nullptr;
    }
};

如果要打分（满分 100 分），我会给 50 分。

扣分主要原因在于一个导致程序完全不可用的致命 Bug，以及一个编译错误。如果没有那个致命 Bug，分数为 75 分。


1. 致命错误 (Critical Bugs)
这是导致你只能拿一半分数的关键原因，代码只要一运行就会出错或内存泄漏。

位置：构造函数

C++

MySharedPtr(T* ptr = nullptr) : rawPtr(ptr) {
    if (ptr)
    {
        b = new Block();
        b->refCount = 1;
    }
    b = nullptr; // <--- ！！！致命错误！！！
}
分析： 无论 ptr 是否为空，你都在构造函数的最后一行强行把 b（控制块指针）设为了 nullptr。

后果：

你刚刚 new Block() 分配的内存瞬间丢失了指针（内存泄漏）。

你的对象虽然持有 rawPtr，但 b 是空的。

当你析构或拷贝时，代码检查 if (b)，发现是空，就不会做引用计数操作，也不会 delete rawPtr。这会导致 rawPtr 指向的对象也发生内存泄漏。

2. 语法错误 (Syntax Error)
位置：Block 定义

C++

struct Block {
    unsigned refCount = 0;
} // <--- 缺少分号
分析： C++ 中结构体定义结束后必须加分号 ;。