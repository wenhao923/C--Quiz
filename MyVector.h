template<typename T>
class MyVector {
public:
    class Iterator;
    MyVector() noexcept = default;

    MyVector(size_t count) : size(count), capacity(count){
        data = static_cast<T*>(::operator new(sizeof(T) * count));

        for (size_t i = 0; i < capacity; i++)
        {
            new (&data[i]) T();
        }
    }

    ~MyVector() noexcept
    { 
        release();
    }

    MyVector(const MyVector&);
    MyVector(MyVector&& other) noexcept : data(other.data), size(other.size), capacity(other.capacity) {
        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    };

    MyVector& operator=(MyVector);

    T& operator[](size_t i) {
        return data[i];
    }

    void push_back(const T&);
    void push_back(T&&);

    template<typename... Args>
    void emplace_back(Args&&...);

    Iterator begin() {
        return Iterator(data);
    };
    Iterator end() {
        return Iterator(data+size);
    };

public:
    class Iterator {
    public:
        Iterator(T* data) : pointer(data) {}
        T& operator*() {
            return *pointer;
        }
        Iterator& operator++() {
            pointer++;
            return *this;
        }

        bool operator!=(Iterator rhs) {
            return pointer != rhs.pointer;
        }
    private:
        T* pointer = nullptr;
    };
    
private:
    T* data = nullptr;
    size_t size = 0;
    size_t capacity = 0;

    void release() noexcept {
        if (data != nullptr) {
            for (size_t i = 0; i < size; ++i) {
                data[i].~T(); 
            }

            ::operator delete(data);
        }

        data = nullptr;
        size = 0;
        capacity = 0;
    }

    void swap(MyVector& other) noexcept {
        std::swap(data, other.data);
        std::swap(size, other.size);
        std::swap(capacity, other.capacity);
    }

    void checkAndExpand() {
        if (size == capacity)
        {
            size_t newCapacity = (size == 0 ? 1 : size * 2);

            auto newData = static_cast<T*>(::operator new(sizeof(T) * newCapacity));
            for (size_t i = 0; i < size; i++)
            {
                new (&newData[i]) T(std::move_if_noexcept(data[i]));
            }

            for (size_t i = 0; i < size; ++i) {
                data[i].~T(); 
            }
        
            ::operator delete(data);

            data = newData;
            capacity = newCapacity;
        }
    }
};

template<typename T>
MyVector<T>::MyVector(const MyVector& other)
{
    data = static_cast<T*>(::operator new(sizeof(T) * other.capacity));    
    
    for (size_t i = 0; i < other.size; i++)
    {
        new (&data[i]) T(other.data[i]);
    }
        
    size = other.size;
    capacity = other.capacity;
}

template<typename T>
MyVector<T>& MyVector<T>::operator=(MyVector rhs)
{
    swap(rhs);
    return *this;
}

template<typename T>
void MyVector<T>::push_back(const T& e) {
    checkAndExpand();
    new (&data[size]) T(e);
    size++;
}

template<typename T>
void MyVector<T>::push_back(T&& e) {
    checkAndExpand();
    new (&data[size]) T(std::move(e));
    size++;
}

template<typename T>
template<typename... Args>
void MyVector<T>::emplace_back(Args&&... args) {
    checkAndExpand();

    new (&data[size]) T(std::forward<Args>(args)...);
    size++;
}