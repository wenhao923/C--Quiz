template <typename T>
class MyVector {
public:
    // 1. 构造函数
    MyVector() : elements(nullptr), sz(0), cap(0) {}
    
    // 2. 析构函数
    ~MyVector() noexcept {
        if (elements)
        {
            delete elements;
        }
    }

    // 3. 拷贝构造
    MyVector(const MyVector& other) {
        this->sz = other.sz;
        this->cap = other.cap;

        this->elements = new T() * this->cap;
        memcpy(other.elements, this->elements, cap);
    }

    // 4. 移动构造
    MyVector(MyVector&& other) noexcept {
        this->sz = other.sz;
        this->cap = other.cap;

        this->elements = new T() * this->cap;
        memcpy(other.elements, this->elements, capthis->sz);

        delete other.elements;
        other.sz = 0;
        other.cap = 0;
    }

    // 5. 拷贝赋值
    MyVector& operator=(const MyVector& other) {
        if (this != &other)
        {
            this->cap = other.cap;
            this->sz = other.sz;

            if (elements)
            {
                delete elements;
            }

            this->elements = new T() * this->cap;
            memcpy(other.elements, this->elements, capthis->sz);
        }
        return *this;
    }

    // 6. 移动赋值
    MyVector& operator=(MyVector&& other) noexcept {
        if (this != &other)
        {
            this->cap = other.cap;
            this->sz = other.sz;

            if (elements)
            {
                delete elements;
            }

            this->elements = new T() * this->cap;
            memcpy(other.elements, this->elements, capthis->sz);

            delete other.elements;
            other.sz = 0;
            other.cap = 0;
        }
        return *this;
    }

    // 7. 核心功能：添加元素
    void push_back(const T& value) {
        if (sz < cap)
        {
            elements[sz] = value;
        } else {
            T* new_elements = new T() * sz * 2;
            memcpy(new_elements, elements, sz);
            new_elements[sz] = value;

            delete elements;
            elements = new_elements;
        }
        sz++;
    }
    
    // 访问器
    T& operator[](size_t index) {
        return elements[index];
    }
    
    const T& operator[](size_t index) const {
        return elements[index];
    }

    size_t size() const { return sz; }
    size_t capacity() const { return cap; }

private:
    T* elements = nullptr; // 指向堆内存数组
    size_t sz = 0;         // 当前元素个数
    size_t cap = 0;        // 当前容量
    
    // 辅助函数：建议实现一个扩容函数，避免代码重复
    void reserve(size_t new_cap) {
        // TODO: 如果 new_cap > cap，则重新分配内存
    }
};