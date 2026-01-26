template <typename T>
class MyVector {
public:
    MyVector() : data(nullptr), capacity(0), size(0) {}
    // 1.构造需要参数么？原生指针、数量？

    ~MyVector() {
        delete data[];
        // 如果data为nullptr会怎样？
    }

    MyVector(const MyVector& other) : data(nullptr), capacity(0), size(0) {
        data = new T[other.capacity];
        for (size_t i = 0; i < other.size; i++) {
            data[i] = other[i];
        }
        capacity = other.capacity;
        size = other.size;
    }

    MyVector(MyVector&& other) : data(nullptr), capacity(0), size(0) {
        data = other[0];

        capacity = other.capacity;
        size = other.size;

        other.data = nullptr;
        other.capacity = 0;
        other.size = 0;
    }

    MyVector& operator=(const MyVector& other) {
        if (this != &other)
        {
            delete data[];

            data = new T[other.capacity];
            for (size_t i = 0; i < other.size; i++) {
                data[i] = other[i];
            }
            capacity = other.capacity;
            size = other.size;
        }
        return *this;
    }

    MyVector& operator=(MyVector&& other) {
        if (this != &other)
        {
            delete data[];

            data = other[0];

            capacity = other.capacity;
            size = other.size;

            other.data = nullptr;
            other.capacity = 0;
            other.size = 0;
        }
        return *this;
    }

    void push_back(T* element) {
        if (capacity > size)
        {
            data[size] = element;
            size++;
        } else {
            T* newData = new T[capacity*2];
            for (size_t i = 0; i < size; i++)
            {
                newData[i] = data[i];
            }
            newData[i] = element;
            capacity *= 2;
            size++;
            data = newData;
        }
    }

    T& operator[](unsigned i) {
        return data[i];
    }

private:
    T* data = nullptr;
    size_t capacity = 0;
    size_t size = 0;
};


这一次的评分是：40分

1. 语法错误：delete 写法 (扣 10 分)
你的代码：

C++

delete data[]; // ❌ 编译报错
分析： C++ 对数组的释放语法是非常严格的，方括号必须紧跟在 delete 后面。 修正：

C++

delete[] data; // ✅
2. 概念错误：Move 是抢“钥匙”，不是抢“家具” (扣 20 分)
你的代码：

C++

// 移动构造
data = other[0]; // ❌ 类型不匹配，且逻辑错误
分析：

data 是 T*（指针，也就是房子的钥匙）。

other[0] 是 T&（第0个元素，也就是房子里的第一个家具）。

编译器会报错： 无法将 T 类型赋值给 T*。

逻辑错误： 移动语义的本质是窃取指针。你应该直接把 other 的指针拿过来，而不是去读它的元素。 修正：

C++

data = other.data; // ✅ 把它的指针直接拿过来
3. 逻辑崩溃：push_back 的三重死局 (扣 20 分)
这是你这段代码里问题最大的函数，有三个致命伤：

无限死循环/越界：

C++

T* newData = new T[capacity*2];
场景：刚开始 capacity 是 0。0 * 2 = 0。

后果：你申请了 0 大小的内存，然后往里面写数据 -> 内存越界崩溃。

变量作用域错误 (编译不过)：

C++

for (size_t i = 0; i < size; i++) { ... }
newData[i] = element; // ❌ 这里的 i 已经未定义了！
i 只在 for 循环里有效。

内存泄漏 (Memory Leak)：

你让 data = newData;，但是旧的 data 指向的内存没释放！旧房子直接丢了。

4. 接口设计问题 (扣 10 分)
你的代码：

C++

void push_back(T* element) // ❌
分析： MyVector<int> 应该存 int，而不是 int*。标准写法是传引用 const T& val。