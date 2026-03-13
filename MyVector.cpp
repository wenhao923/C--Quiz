#include "MyVector.h"

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