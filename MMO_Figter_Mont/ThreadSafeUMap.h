#include <iostream>
#include <unordered_map>
#include <mutex>
#include <thread>

template<typename Key, typename Value>
class ThreadSafeUnorderedMap {
private:
    std::unordered_map<Key, Value> map_;
    mutable std::mutex mutex_;

public:
    // Insert key-value pair into the map
    void insert(const Key& key, const Value& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        map_[key] = value;
    }

    // Retrieve value associated with the key
    // If the key does not exist, return false
    bool find(const Key& key, Value& value) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            value = it->second;
            return true;
        }
        return false;
    }

    // Erase key-value pair from the map
    //void erase(const Key& key) {
    //    std::lock_guard<std::mutex> lock(mutex_);
    //    map_.erase(key);
    //}
    typename std::unordered_map<Key, Value>::iterator erase(const Key& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            return map_.erase(it);
        }
        return map_.end(); // 반환값이 end를 가리키는 경우는 찾을 수 없는 경우
    }
    typename std::unordered_map<Key, Value>::iterator erase(typename std::unordered_map<Key, Value>::iterator it) {
        std::lock_guard<std::mutex> lock(mutex_);
        return map_.erase(it);
    }

    // Check if the map is empty
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return map_.empty();
    }

    // Thread-safe iterator for beginning of map
    typename std::unordered_map<Key, Value>::const_iterator begin() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return map_.begin();
    }

    // Thread-safe iterator for end of map
    typename std::unordered_map<Key, Value>::const_iterator end() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return map_.end();
    }
};
