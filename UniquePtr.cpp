template<typename T>
class MyUniquePtr {
public:
    MyUniquePtr(T* ptr = nullptr): rawPtr(ptr) {}
    ~MyUniquePtr() { 
        if (rawPtr)
        {
            delete rawPtr;
            rawPtr = nullptr;
        }
    }

    MyUniquePtr(const MyUniquePtr&) = delete;
    MyUniquePtr(MyUniquePtr&& other) noexcept {
        rawPtr = other.rawPtr;
        other.rawPtr = nullptr;
    }

    MyUniquePtr& operator=(const MyUniquePtr&) = delete;
    MyUniquePtr& operator=(MyUniquePtr&& other) noexcept {
        if (this != &other)
        {
            rawPtr = other.rawPtr;
            other.rawPtr = nullptr;
        }
        return *this;
    }

    T* get() const noexcept { return rawPtr; }
    T* operator->() const noexcept { return rawPtr; }
    T& operator*() const noexcept { return *rawPtr; }
private:
    T* rawPtr = nullptr;
};