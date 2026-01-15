struct Block {
    unsigned refCount = 0;
};

template <typename T>
class MySharedPtr {
public:
    MySharedPtr(T* ptr) : rawPtr(ptr) {
        b = new Block();
        b->refCount = 1;
    }
    ~MySharedPtr() {
        b->refCount--;
        if (b->refCount <= 0)
        {
            delete rawPtr;
        }
    }

    MySharedPtr(const MySharedPtr& other) noexcept {
        b = other.b;
        rawPtr = other.rawPtr;

        b->refCount++;
    }

    MySharedPtr(MySharedPtr&& other) noexcept {
        b = other.b;
        rawPtr = other.rawPtr;

        other.rawPtr = nullptr;
        other.b = nullptr;
    }

    explicit MySharedPtr& operator=(const MySharedPtr& other) noexcept {
        if (this != &other)
        {
            b = other.b;
            rawPtr = other.rawPtr;

            b->refCount++;
        }
        return *this;   
    }

    explicit MySharedPtr& operator=(MySharedPtr&& other) noexcept {
        if (this != &other)
        {
            b = other.b;
            rawPtr = other.rawPtr;

            other.rawPtr = nullptr;
            other.b = nullptr;
        }
        return *this;   
    }

    T* get() const noexcept { 
        if (b && b->refCount > 0)
        {
            return rawPtr; 
        }
        return nullptr;
    }
    T* operator->() const noexcept { 
        if (b && b->refCount > 0)
        {
            return rawPtr; 
        }
        return nullptr;
    }
    T& operator*() const noexcept { 
        if (b && b->refCount > 0)
        {
            return *rawPtr; 
        }
        return nullptr;
    }

private:
    T* rawPtr;
    Block* b;
};