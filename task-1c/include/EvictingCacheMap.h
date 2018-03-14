#ifndef LRU_EVICTINGCACHEMAP_H
#define LRU_EVICTINGCACHEMAP_H

#include <functional>
#include <utility>
#include <optional>
#include <vector>
#include <list>
#include <stdexcept>
#include <utility>

template <class TKey, class TValue, class THash = std::hash<TKey>>
class EvictingCacheMap final {
public:
    using value_type = std::pair<const TKey, TValue>;

    using iterator = typename std::list<value_type>::iterator;
    using const_iterator = typename std::list<value_type>::const_iterator;

    /**
     * Construct a EvictingCacheMap
     * @param capacity maximum size of the cache map.  Once the map size exceeds
     *    maxSize, the map will begin to evict.
    */
    explicit EvictingCacheMap(std::size_t capacity)
            : hashTable(capacity), capacity(capacity) {
    }

    EvictingCacheMap(const EvictingCacheMap & other) {
        *this = other;
    }

    EvictingCacheMap(EvictingCacheMap && other) noexcept {
        *this = std::move(other);
    }

    ~EvictingCacheMap() = default;

    EvictingCacheMap & operator=(const EvictingCacheMap & other) {
        if (this == &other)
            return *this;

        capacity = other.capacity;

        clear();    //  hashTable.size() == capacity

        for (auto it = other.list.rbegin(); it != other.list.rend(); ++it) {
            put(it->first, it->second);
        }

        return *this;
    }

    EvictingCacheMap & operator=(EvictingCacheMap && other) noexcept {
        if (this == &other)
            return *this;

        list = std::move(other.list);
        hashTable = std::move(other.hashTable);

        capacity = other.capacity;

        return *this;
    }

    /**
     * Check for existence of a specific key in the map.  This operation has
     *     no effect on LRU order.
     * @param key key to search for
     * @return true if exists, false otherwise
    */
    bool exists(const TKey & key) const {
        if (capacity == 0)
            return false;

        auto & bucket = hashTable[keyToBucket(key)];
        for (auto & it : bucket) {
            if (it->first == key)
                return true;
        }

        return false;
    }

    /**
     * Get the value associated with a specific key.  This function always
     *     promotes a found value to the head of the LRU.
     * @param key key associated with the value
     * @return the value if it exists
    */
    std::optional<TValue> get(const TKey & key) {
        auto it = find(key);
        if (it == end())
            return {};

        return it->second;
    }

    /**
     * Get the iterator associated with a specific key.  This function always
     *     promotes a found value to the head of the LRU.
     * @param key key to associate with value
     * @return the iterator of the object (a std::pair of const TKey, TValue) or
     *     end() if it does not exist
    */
    iterator find(const TKey & key) {
        if (capacity == 0)
            return end();

        auto & bucket = hashTable[keyToBucket(key)];
        for (auto & it : bucket) {
            if (it->first != key)
                continue;

            list.splice(list.begin(), list, it);

            return it;
        }

        return end();
    }

    /**
     * Erase the key-value pair associated with key if it exists.
     * @param key key associated with the value
     * @return true if the key existed and was erased, else false
    */
    bool erase(const TKey & key) {
        if (capacity == 0)
            return false;

        auto & bucket = hashTable[keyToBucket(key)];
        for (auto itB = bucket.begin(); itB != bucket.end(); ++itB) {
            if ((*itB)->first != key)
                continue;

            auto & it = *itB;
            bucket.erase(itB);
            list.erase(it);

            return true;
        }

        return false;
    }

    /**
     * Set a key-value pair in the dictionary
     * @param key key to associate with value
     * @param value value to associate with the key
     */
    template <class T, class E>
    void put(T && key, E && value) {
        if (capacity == 0)
            return;

        auto it = find(key);
        if (it != list.end()) {
            it->second = std::forward<E>(value);
            return;
        }

        auto & bucket = hashTable[keyToBucket(key)];
        list.emplace_front(std::forward<T>(key), std::forward<E>(value));
        bucket.emplace_front(list.begin());

        if (list.size() > capacity)
            erase(list.rbegin()->first);

    if (bucket.size() > BUCKET_SIZE)
        rehash();
    }

    /**
     * Get the number of elements in the dictionary
     * @return the size of the dictionary
    */
    std::size_t size() const {
        return list.size();
    }

    /**
     * Typical empty function
     * @return true if empty, false otherwise
    */
    bool empty() const {
        return list.empty();
    }

    void clear() {
        hashTable = std::vector<std::list<iterator>>(capacity);
        list.clear();
    }

    // Iterators and such
    iterator begin() noexcept {
        return list.begin();
    }

    iterator end() noexcept {
        return list.end();
    }

    const_iterator begin() const noexcept {
        return list.begin();
    }

    const_iterator end() const noexcept {
        return list.end();
    }

    const_iterator cbegin() const noexcept {
        return list.cbegin();
    }

    const_iterator cend() const noexcept {
        return list.cend();
    }

private:
    static constexpr const std::size_t BUCKET_SIZE = 1;

    std::list<value_type> list;
    std::vector<std::list<iterator>> hashTable;

    std::size_t capacity = 0;

    std::size_t keyToBucket(const TKey & key) const noexcept {
        return THash()(key) % hashTable.size();
    }

    void rehash() {
        auto newTable = std::vector<std::list<iterator>>(hashTable.size() * 2);

        for (auto & bucket : hashTable) {
            while (!bucket.empty()) {
                auto itB = bucket.begin();
                auto & newBucket = newTable[THash()((*itB)->first) % newTable.size()];
                newBucket.splice(newBucket.begin(), newBucket, itB);
            }
        }

        hashTable = std::move(newTable);
    }
};

#endif //LRU_EVICTINGCACHEMAP_H
