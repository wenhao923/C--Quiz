template<typename T>
class MyVector {
public:
    MyVector() {}
    ~MyVector() { delete[] data; }  // delete[] 在做什么事情？
    MyVector(const MyVector&);
    MyVector(MyVector&&);
    MyVector& operator=(const MyVector&);
    MyVector& operator=(MyVector&&);
    void push_back(const T&);
private:
    T* data = nullptr;
    size_t capacity = 0;
    size_t size = 0;
};

template<typename T>
MyVector<T>::MyVector(const MyVector& other)
{
    capacity = other.capacity;
    size = other.size;
    data = new T[capacity];
    for (size_t i = 0; i < size; i++)
    {
        data[i] = other.data[i];    // T的拷贝赋值，平凡类直接赋值（浅拷贝？），其他的需要深拷贝
    }
}

template<typename T>
MyVector<T>::MyVector(MyVector&& other) {
    capacity = other.capacity;
    size = other.size;
    data = other.data;

    other.capacity = 0;
    other.size = 0;
    other.data = nullptr;
}

template<typename T>
MyVector<T>& MyVector<T>::operator=(const MyVector& other) {    // 为什么定义返回类型加<T>, 参数不加、声明不加
    if (this != other)
    {
        delete[] data;

        capacity = other.capacity;
        size = other.size;
        data = new T[capacity];

        for (size_t i = 0; i < size; i++)
        {
            data[i] = other.data[i];
        }
    }
    return *this;
}

template<typename T>
MyVector<T>& MyVector<T>::operator=(MyVector&& other) {
    if (this != other)
    {
        delete[] data;

        capacity = other.capacity;
        size = other.size;
        data = other.data;

        other.capacity = 0;
        other.size = 0;
        other.data = nullptr;
    }
    return *this;
}

template<typename T>
void MyVector<T>::push_back(const T& e) {
    if (size == capacity)
    {
        size_t newCapacity = capacity == 0 ? 1 : capacity * 2;
        T* newData = new T[newCapacity];
        for (size_t i = 0; i < size; i++)
        {
            newData[i] = std::move(data[i]);
        }

        capacity = newCapacity;
        delete[] data;
        data = newData;
    }
    data[i++] = std::move(e);   // 这里move有用么？e是栈上定义的话，还能move并存入么？如果是堆上定义的呢？
}

85分

1. 代码纠错 (Bug Fixes)
❌ 错误 1：指针与引用的比较
你的代码：

C++

if (this != other) // error: invalid operands to binary expression ('MyVector<T> *' and 'const MyVector<T>')
分析：

this 是一个指针（Pointer）。

other 是一个引用（Reference）。

指针不能直接和引用比较。 修正： 必须取 other 的地址。

C++

if (this != &other)

❌ 错误 2：变量未定义
你的代码：

C++

// 在 push_back 最后
data[i++] = std::move(e); // error: use of undeclared identifier 'i'
分析： i 是你在 if 块里定义的局部变量，出了 if 就没了。这里应该用类的成员变量 size。 修正：

C++

data[size++] = std::move(e);