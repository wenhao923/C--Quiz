#include <list>
#include <unordered_map>
class LRUCache {
public:
    LRUCache(int capacity) {
        mCapacity = capacity;
    }

    int get(int key) {
        int result = -1;
        if (mMap.find(key) != mMap.end())
        {
            result = mMap[key]->second;
            mList.splice(mList.begin(), mList, mMap[key]);
        }
        return result;
    }

    void put(int key, int value) {
        if (mMap.find(key) != mMap.end()) {
            mMap[key]->second = value;
            mList.splice(mList.begin(), mList, mMap[key]);
        } else if (mMap.size() < mCapacity) {
            mList.push_front(std::make_pair(key, value));
            mMap[key] = mList.begin();
        } else {
            int toRemove = mList.back().first;
            mMap.erase(toRemove);
            mList.pop_back();
            
            mList.push_front(std::make_pair(key, value));
            mMap[key] = mList.begin();
        }

    }
private:
    unsigned mCapacity = 0;
    std::list<std::pair<int, int>> mList;
    using tListIter = std::list<std::pair<int, int>>::iterator;
    std::unordered_map<int, tListIter> mMap;
};


❌ 致命错误：删了 Value，没删 Key
你的代码：

C++

// 你的 list 存的是 pair<Key, Value>
// back().first 是 Key
// back().second 是 Value

int toRemove = mList.back().second; // <--- 错误！你拿到了 Value
mMap.erase(toRemove);               // <--- 错误！你试图从 Map 里删除 Value


还有一个扣分项：双重查找
你在 get 和 put 里依然保留了 mMap[key] 这种写法。 虽然逻辑对了（修好上面的 .first 后），但在网易引擎组的面试官眼里，这叫**“性能洁癖不足”**。

mMap.find(key) 算了一次哈希。

mMap[key] 又算了一次哈希。

引擎开发原则：能只算一次，绝不算两次。