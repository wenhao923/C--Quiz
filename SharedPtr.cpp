struct Block {
    unsigned refCount = 0;
};

template <typename T>
class MySharedPtr {
public:
    MySharedPtr(T* ptr) : rawPtr(ptr) {
        b = new Block();
        b->refCount = 1;
    }
    ~MySharedPtr() {
        b->refCount--;
        if (b->refCount <= 0)
        {
            delete rawPtr;
        }
    }

    MySharedPtr(const MySharedPtr& other) noexcept {
        b = other.b;
        rawPtr = other.rawPtr;

        b->refCount++;
    }

    MySharedPtr(MySharedPtr&& other) noexcept {
        b = other.b;
        rawPtr = other.rawPtr;

        other.rawPtr = nullptr;
        other.b = nullptr;
    }

    explicit MySharedPtr& operator=(const MySharedPtr& other) noexcept {
        if (this != &other)
        {
            b = other.b;
            rawPtr = other.rawPtr;

            b->refCount++;
        }
        return *this;   
    }

    explicit MySharedPtr& operator=(MySharedPtr&& other) noexcept {
        if (this != &other)
        {
            b = other.b;
            rawPtr = other.rawPtr;

            other.rawPtr = nullptr;
            other.b = nullptr;
        }
        return *this;   
    }

    T* get() const noexcept { 
        if (b && b->refCount > 0)
        {
            return rawPtr; 
        }
        return nullptr;
    }
    T* operator->() const noexcept { 
        if (b && b->refCount > 0)
        {
            return rawPtr; 
        }
        return nullptr;
    }
    T& operator*() const noexcept { 
        if (b && b->refCount > 0)
        {
            return *rawPtr; 
        }
        return nullptr;
    }

private:
    T* rawPtr;
    Block* b;
};

评分: 40

❌ 扣分点 1：严重的内存泄漏 (Control Block Leak) [-20分]
你的代码：

C++

if (b->refCount <= 0) {
    delete rawPtr; // 删了数据
    // 😱 完了，Block b 也是 new 出来的，谁来 delete 它？
}
判词： shared_ptr 是“一拖二”的结构：堆上有数据，也有控制块。 你只管了数据的死活，没管控制块。每次一个对象彻底销毁，堆上就会残留一个 Block 结构体。跑个几万次，内存就满了。

正确写法：

C++

if (b->refCount == 0) {
    delete rawPtr;
    delete b; // ✅ 必须把计数器自己也删了！
}
❌ 扣分点 2：赋值操作导致的泄漏 (Assignment Leak) [-20分]
你的代码：

C++

// operator=
if (this != &other) {
    // 😱 你直接把 b 和 rawPtr 覆盖了！
    // 那你手里原来拿的那个对象的引用计数谁去减？
    b = other.b; 
    rawPtr = other.rawPtr;
    b->refCount++; 
}
判词： 这是写智能指针最大的忌讳。赋值操作本质是 “分手 + 恋爱”。 你现在的逻辑是：直接去谈新恋爱，完全不管前任（原来的 refCount 没减）。

后果：你原来的前任（对象）引用计数永远不会归零，永远无法释放。这是 100% 的资源泄露。

正确逻辑：

旧的 refCount--。

如果旧的归零，删除旧资源。

接管新的，新的 refCount++。

❌ 扣分点 3：移动后的崩溃 (Move Crash) [-10分]
你的代码：

C++

// 假设 p1 被移动给了 p2，p1.b 变成了 nullptr
~MySharedPtr() {
    b->refCount--; // 💥 崩！访问 nullptr->refCount
}
判词： 移动语义会将源对象置空。析构函数必须检查 b 是否为空，否则操作空指针直接导致程序崩溃。

❌ 扣分点 4：语法与逻辑错误 [-10分]
explicit operator=：

错。explicit 不能修饰赋值运算符，编译器会直接报错。它只能修饰构造函数和类型转换函数。

T& operator* 返回 nullptr：

错。C++ 中引用必须绑定到合法对象，不能返回空。这行代码甚至可能无法通过编译，或者导致未定义行为。