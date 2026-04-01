#include "MyVector.h"

template<typename T>
using Vector = MyVector<T>;

template<typename K, typename V>
struct HashNode
{
    K key_;
    V value_;
    HashNode* next_ = nullptr;

    HashNode(const K& k, const V& v, HashNode* p): key_(k), value_(v), next_(p) {};
};

template<typename K, typename V>
class MyUnorderedMap {
public:
    const V* find(const K&) noexcept;
    void insert(const K&, const V&);
    void erase(const K&) noexcept;

    MyUnorderedMap() : buckets_(), num_elements_(0) {};    // 要使用nullptr初始化Vector函数么
    ~MyUnorderedMap();

private:
    size_t get_bucket_index(const K& key, size_t bucket_count) {
        size_t hash_val = std::hash<K>{}(key);
        return hash_val % bucket_count;
    }

    void rehash();

    Vector<HashNode<K, V>*> buckets_;    // 容器桶
    size_t num_elements_;                // 元素（节点）总数

    const float loadFactor = 1.0;       // 负载因子
    const size_t INITIAL_CAPACITY = 8;  // 初始化容量
};

template<typename K, typename V>
void MyUnorderedMap<K, V>::rehash() {
    size_t newBucketsNum = buckets_.size() * 2;
    Vector<HashNode<K, V>*> newBuckets(newBucketsNum);

    // 将链表A节点移动到链表B
    for (size_t i = 0; i < buckets_.size(); i++)
    {
        HashNode<K, V>* curNode = buckets_[i];
        while (curNode)
        {
            HashNode<K, V>* next = curNode->next_;

            size_t index = get_bucket_index(curNode->key_, newBucketsNum);
            curNode->next_ = newBuckets[index];
            newBuckets[index] = curNode;

            curNode = next;
        }
    }
    
    buckets_ = std::move(newBuckets);
}

template<typename K, typename V>
const V* MyUnorderedMap<K, V>::find(const K& key) noexcept {
    if (buckets_.size() == 0)
        return nullptr;
    
    size_t index = get_bucket_index(key, buckets_.size());

    HashNode<K, V> *resNode = buckets_[index];
    while (resNode && resNode->key_ != key) {
        resNode = resNode->next_;
    }
    if (resNode)
        return &resNode->value_;
    else
        return nullptr;
}

template<typename K, typename V>
void MyUnorderedMap<K, V>::insert(const K& key, const V& value) {
    // lazy初始化
    if (buckets_.size() == 0)
    {
        buckets_ = Vector<HashNode<K, V>*>(INITIAL_CAPACITY);
    }

    // key重复覆盖
    size_t index = get_bucket_index(key, buckets_.size());
    HashNode<K, V>* curr = buckets_[index];
    while (curr) {
        if (curr->key_ == key)
        {
            curr->value_ = value;
            return;
        }
        curr = curr->next_;
    }

    // 重新映射
    if (num_elements_+1 >= buckets_.size() * loadFactor)
    {
        rehash();
    }

    index = get_bucket_index(key, buckets_.size());
    // 链表头插法
    buckets_[index] = new HashNode<K, V>(key, value, buckets_[index]);
    num_elements_++;
}

template<typename K, typename V>
void MyUnorderedMap<K, V>::erase(const K& key) noexcept {
    if (buckets_.size() == 0) return;

    size_t index = get_bucket_index(key, buckets_.size());

    // 删除指定链表节点, 工业级不要创建dummyNode（可能构造大对象），用轻量级的指针
    HashNode<K, V> *prev = nullptr;
    HashNode<K, V> *curr = buckets_[index];

    while (curr) {
        if (curr->key_ == key)
        {
            if (prev)
            {
                prev->next_ = curr->next_;
            } else {
                buckets_[index] = curr->next_;
            }

            delete curr;
            num_elements_--; 
            return;
        }

        prev = curr;
        curr = curr->next_;
    }
}

template<typename K, typename V>
MyUnorderedMap<K, V>::~MyUnorderedMap() {
    for (size_t i = 0; i < buckets_.size(); i++)
    {
        HashNode<K, V>* curNode = buckets_[i];
        while (curNode)
        {
            HashNode<K, V>* nxt = curNode->next_;
            delete curNode;
            curNode = nxt;
        }
    }
    num_elements_ = 0;
}

/* 一些问题
    // HashNode是使用外部insert的node还是copy一份？外部的内存不受控制，内存被释放了怎么办？
    // std::hash是什么用法?
    // struct HashNode 需要定义构造函数么？默认是什么？
*/