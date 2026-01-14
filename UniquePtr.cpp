template<typename T>
class MyUniquePtr {
public:
    MyUniquePtr(T* ptr) { rawPtr = ptr;}
    ~MyUniquePtr() {
        delete rawPtr;
        rawPtr = nullptr;
    }

    MyUniquePtr(const MyUniquePtr&) = delete;
    MyUniquePtr& operator=(const MyUniquePtr&) = delete;

    MyUniquePtr(MyUniquePtr&& other) noexcept {
        rawPtr = other.rawPtr;
        other.rawPtr = nullptr;
    }

    MyUniquePtr& operator=(MyUniquePtr&& other) noexcept {
        if (rawPtr != other.rawPtr)
        {
            rawPtr = other.rawPtr;
            other.rawPtr = nullptr;
        }
        return *this;
    }

    T* get() const { return rawPtr; }
    T* operator->() const { return rawPtr; }
    T& operator*() const { return *rawPtr; }
private:
    T* rawPtr;
};

// =====================================
☠️ 致命错误 1：移动赋值里的“喜新厌旧” (Memory Leak)
你的代码：

C++

MyUniquePtr& operator=(MyUniquePtr&& other) noexcept {
    if (rawPtr != other.rawPtr) {
        // ❌ 致命问题在这里！
        // 你直接把 rawPtr 覆盖了，那你手里原本拿的那个旧指针呢？
        // 它变成了孤儿，永远没人 delete 它了 -> 内存泄漏！
        rawPtr = other.rawPtr; 
        other.rawPtr = nullptr;
    }
    return *this;
}
场景模拟： 你手里拿着一个苹果 (rawPtr 指向 Apple)，现在你要接管别人的香蕉 (other.rawPtr 指向 Banana)。 你现在的逻辑是：直接扔掉手里的苹果（但不扔进垃圾桶，而是随地乱扔），然后把香蕉拿过来。 久而久之，地上全是烂苹果。

修正： 必须先 delete 自己手里的旧东西！

C++

delete rawPtr; // ✅ 先清理旧账
rawPtr = other.rawPtr;

⚠️ 逻辑隐患 2：自赋值检查的对象搞错了
你的代码：

C++

if (rawPtr != other.rawPtr) // ⚠️ 比较的是“钻石”，不是“箱子”
问题分析： 虽然在 unique_ptr 的逻辑里，不同的智能指针不应该管理同一个 rawPtr，所以你这样写在大多数正常情况下能防住自赋值。 但是，标准的做法是比较智能指针对象本身的地址，而不是它们管理的资源地址。

场景： ptr = std::move(ptr); 这时候 this 和 &other 是同一个地址。

修正：

C++

if (this != &other) // ✅ 比较箱子的地址

⚠️ 安全隐患 3：缺少 explicit
你的代码：

C++

MyUniquePtr(T* ptr) { rawPtr = ptr;}
问题： 我们在前几轮讨论过，单参数构造函数必须加 explicit。否则： void func(MyUniquePtr<int> p); 调用 func(new int(1)); 会导致隐式转换，这是不安全的。


⚠️ 性能隐患 3：初始化列表
虽然对于你的 MyUniquePtr（内部只是个裸指针 T*）来说，两者的性能在汇编层面几乎是一样的；但如果成员变量是一个复杂的类（比如 std::string 或 std::vector），区别就非常惊人了。