template<typename T>
class MyVector {
public:
    MyVector() {}
    ~MyVector() noexcept { delete[] data; }  // delete[] 在做什么事情？
    MyVector(const MyVector&);
    MyVector(MyVector&&) noexcept;
    MyVector& operator=(MyVector) noexcept;
    MyVector& operator=(MyVector&&) noexcept;
    void push_back(const T&);
private:
    void swap(MyVector& rhs) {
        using std::swap;
        swap(data, rhs.data);
        swap(capacity, rhs.capacity);
        swap(size, rhs.size);
    }

    T* data = nullptr;
    size_t capacity = 0;
    size_t size = 0;
};

template<typename T>
MyVector<T>::MyVector(const MyVector& other)
{
    data = new T[capacity];
    capacity = other.capacity;
    size = other.size;
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
MyVector<T>& MyVector<T>::operator=(MyVector rhs) {
    this->swap(rhs);
    
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
    data[i++] = std::move(e);
}
