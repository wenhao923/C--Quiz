template <typename T>
class MyVector{
public:
    MyVector() {}
    ~MyVector() {
        delete[] element;
    }
    MyVector(const MyVector& other) : size(other.size), capacity(other.capacity) {
        element = new T[capacity];
        for (size_t i = 0; i < size; i++)
        {
            element[i] = other.element[i]; // T的赋值函数要做好？
        }
    }
    MyVector(MyVector&& other) : element(other.element), size(other.size), capacity(other.capacity) {
        other.element = nullptr;
        other.size = 0;
        other.capacity = 0;
    }

    MyVector& operator=(const MyVector& other) : size(other.size), capacity(other.capacity) {
        if (this != &other)
        {
            delete[] element;

            element = new T[capacity];
            for (size_t i = 0; i < size; i++)
            {
                element[i] = other.element[i]; // T的赋值函数要做好？
            }
        }
        return *this;
    }

    MyVector& operator=(MyVector&& other) {
        if (this != &other)
        {
            delete[] element;

            element = other.element;
            size = other.size;
            capacity = other.capacity;

            other.element = nullptr;
            other.size = 0;
            other.capacity = 0;
        }
        return *this;
    }

    void push_back(const T& e) {
        if (size == capacity)
        {
            NewElement = new T[size == 0 ? 1 : capacity * 2];
            for (size_t i = 0; i < size; i++)
            {
                NewElement[i] = element[i]; // T的赋值函数要做好？
            }
            delete[] element;
            element = NewElement;   // 可以在原element后面拓展内存么？
        }

        element[size] = &e;
        size++;
    }
private:
    T* element = nullptr;   // 可以表示多个数量么？
    size_t size = 0;
    size_t capacity = 0;
};


60分

❌ 错误 1：赋值运算符不能用“初始化列表”
你的代码：

C++

MyVector& operator=(const MyVector& other) : size(other.size), capacity(other.capacity) { // 错误！
分析： 冒号后面的初始化列表（: size(...)）只能用在构造函数里。赋值函数（operator=）是普通函数，只能在函数体的大括号里写赋值语句。 修正：

C++

MyVector& operator=(const MyVector& other) {
    // ... 前面的检查 ...
    size = other.size;         // 写在函数体里
    capacity = other.capacity; // 写在函数体里
}

❌ 错误 2：push_back 类型不匹配
你的代码：

C++

element[size] = &e; // 错误！
分析：

element[size] 的类型是 T（对象本身）。

&e 的类型是 T*（对象的地址）。

你不能把一个地址赋给一个对象。 修正：

C++

element[size] = e; // 正确，直接拷贝值

❌ 错误 3：push_back 忘记更新 capacity
你的代码：

C++

// 在扩容逻辑里
NewElement = new T[...];
// ... 搬运 ...
element = NewElement;
// 结束了？capacity 还没变呢！

❌ 错误 4：变量未声明
你的代码：

C++

NewElement = new T[...]; // NewElement 是啥类型？
分析： C++ 是强类型语言，必须声明类型。 修正：

C++

T* NewElement = new T[...];