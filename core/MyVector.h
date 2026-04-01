#pragma once
#include <iostream>
namespace INTERNAL { 
    template<typename T>
    class MyVectorIterator {
    public:
        MyVectorIterator(T* data_) : pointer(data_) {}
        T& operator*() {
            return *pointer;
        }
        MyVectorIterator& operator++() {
            pointer++;
            return *this;
        }

        bool operator!=(MyVectorIterator rhs) {
            return pointer != rhs.pointer;
        }
    private:
        T* pointer = nullptr;
    }; 
}

template<typename T>
class MyVector {
public:
    using Iterator = INTERNAL::MyVectorIterator<T>;

    MyVector() noexcept = default;

    MyVector(size_t count) : size_(count), capacity_(count){
        data_ = static_cast<T*>(::operator new(sizeof(T) * count));

        for (size_t i = 0; i < capacity_; i++)
        {
            new (&data_[i]) T();
        }
    }

    ~MyVector() noexcept
    { 
        release();
    }

    MyVector(const MyVector&);
    MyVector(MyVector&& other) noexcept : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    };

    MyVector& operator=(MyVector);

    T& operator[](size_t i) {
        return data_[i];
    }

    size_t size() const {
        return size_;
    }

    void push_back(const T&);
    void push_back(T&&);

    template<typename... Args>
    void emplace_back(Args&&...);

    Iterator begin() {
        return Iterator(data_);
    };
    Iterator end() {
        return Iterator(data_ + size_);
    };
    
private:
    T* data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;

    void release() noexcept {
        if (data_ != nullptr) {
            for (size_t i = 0; i < size_; ++i) {
                data_[i].~T(); 
            }

            ::operator delete(data_);
        }

        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }

    void swap(MyVector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    void checkAndExpand() {
        if (size_ == capacity_)
        {
            size_t newCapacity = (size_ == 0 ? 1 : size_ * 2);

            auto newData = static_cast<T*>(::operator new(sizeof(T) * newCapacity));
            for (size_t i = 0; i < size_; i++)
            {
                new (&newData[i]) T(std::move_if_noexcept(data_[i]));
            }

            for (size_t i = 0; i < size_; ++i) {
                data_[i].~T(); 
            }
        
            ::operator delete(data_);

            data_ = newData;
            capacity_ = newCapacity;
        }
    }
};

template<typename T>
MyVector<T>::MyVector(const MyVector& other)
{
    data_ = static_cast<T*>(::operator new(sizeof(T) * other.capacity_));    
    
    for (size_t i = 0; i < other.size_; i++)
    {
        new (&data_[i]) T(other.data_[i]);
    }
        
    size_ = other.size_;
    capacity_ = other.capacity_;
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
    new (&data_[size_]) T(e);
    size_++;
}

template<typename T>
void MyVector<T>::push_back(T&& e) {
    checkAndExpand();
    new (&data_[size_]) T(std::move(e));
    size_++;
}

template<typename T>
template<typename... Args>
void MyVector<T>::emplace_back(Args&&... args) {
    checkAndExpand();

    new (&data_[size_]) T(std::forward<Args>(args)...);
    size_++;
}