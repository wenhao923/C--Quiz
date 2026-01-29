template <typename T>
class MyVector {
public:
    MyVector() {};
    ~MyVector() { delete[] data; }
    MyVector(const MyVector&);
    MyVector(MyVector&&);
    MyVector& operator=(const MyVector&);
    MyVector& operator=(MyVector&&);
    void push_back(const T&);
private:
    T* data = nullptr;
    size_t capacity = 0;
    size_t size = 0;    // 为什么用size_t？
};

template <typename T>
MyVector::MyVector(const MyVector& other)
{
    capacity = other.capacity;
    size = other.size;

    data = new T[capacity];
    for (size_t i = 0; i < count; i++) {
        data[i] = other[i];
    }
}

template <typename T>
MyVector::MyVector(MyVector&& other)
{
    capacity = other.capacity;
    size = other.size;
    data = other.data;

    other.capacity = 0;
    other.size = 0;
    other.data = nullptr;
}

template <typename T>
MyVector& MyVector::operator=(const MyVector& other)
{
    if (this != &other)
    {
        capacity = other.capacity;
        size = other.size;

        data = new T[capacity];
        for (size_t i = 0; i < count; i++) {
            data[i] = other[i];
        }
    }
    return *this;
}

template <typename T>
MyVector& MyVector::operator=(MyVector&& other)
{
    if (this != &other)
    {
        capacity = other.capacity;
        size = other.size;
        data = other.data;

        other.capacity = 0;
        other.size = 0;
        other.data = nullptr;
    }
    return *this;
}

template <typename T>
void push_back(const T& e) 
{
    if (size == capacity)
    {
        size_t newCap = capcity == 0 ? 1 : capacity * 2;
        T* newData = new T[newCap];
        capcacity = newCap;
        
        for (size_t i = 0; i < count; i++) {
            newData[i] = std::move(data[i]);   //调用T的赋值构造？
        }
        delete data[];
    }
    data[size++] = std::move(e);   //调用T的赋值构造？
}

1. 语法灾难：类外实现的模板写法
你的代码：

C++

template <typename T>
MyVector::MyVector(const MyVector& other) { ... } // ❌ 编译错误
分析： MyVector 是一个模板类，不是普通类。在类外写函数时，必须告诉编译器它是属于 MyVector<T> 的。 修正：

C++

template <typename T>
MyVector<T>::MyVector(const MyVector& other) { ... } // ✅ 加上 <T>
所有类外函数都要改！

2. 逻辑错误：赋值运算符里的内存泄漏
你的代码：

C++

// 拷贝赋值 & 移动赋值
if (this != &other) {
    // 这里还没 delete[] data 呢！
    data = new T[capacity]; // ❌ 原来的 data 指向的内存丢了（泄漏）
    // ...
}
修正： 必须先 delete[] data，再接管新内存或新指针。

2. 逻辑错误：赋值运算符里的内存泄漏
你的代码：

C++

// 拷贝赋值 & 移动赋值
if (this != &other) {
    // 这里还没 delete[] data 呢！
    data = new T[capacity]; // ❌ 原来的 data 指向的内存丢了（泄漏）
    // ...
}
修正： 必须先 delete[] data，再接管新内存或新指针。

3. 变量未定义
你的代码：

C++

for (size_t i = 0; i < count; i++) // ❌ count 是谁？
修正： 应该是 size。

4. push_back 的致命 Bug (老问题)
你的代码：

C++

delete data[]; // ❌ 语法错误
// 漏了一行关键代码！
分析： 你删了旧 data，但是没把 newData 给 data。data 变成了野指针。 修正： delete[] data; data = newData;