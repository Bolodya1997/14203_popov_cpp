#ifndef LRU_EVICTINGCACHEMAP_H
#define LRU_EVICTINGCACHEMAP_H

#include <functional>
#include <utility>
#include <optional>
#include <vector>
#include <list>
#include <stdexcept>

template <class TKey, class TValue, class THash = std::hash<TKey>>
class EvictingCacheMap final {
    struct Node;

public:
    class iterator: public std::iterator<std::input_iterator_tag,
            std::pair<const TKey, TValue>> {
    public:
        bool operator==(const iterator & other) const {
            return node == other.node;
        }

        bool operator!=(const iterator & other) const {
            return !(*this == other);
        }

        std::pair<const TKey, TValue> & operator*() {
            return dereference();
        }

        std::pair<const TKey, TValue> & operator->() {
            return **this;
        }

        iterator & operator++() {
            if (node == nullptr)
                throw std::out_of_range(OUT_OF_RANGE);
            node = node->next;

            return *this;
        }

        iterator & operator++(int) {
            iterator res = *this;
            ++*this;

            return res;
        }

    private:
        static const std::string OUT_OF_RANGE;

        iterator(Node * node)
                : node(node) {
        }

        Node * node = nullptr;

        std::pair<const TKey, TValue> & dereference() const {
            if (node == nullptr)
                throw std::out_of_range(OUT_OF_RANGE);
            return node->data;
        }

        friend class const_iterator;
        friend class EvictingCacheMap;
    };

    class const_iterator: public std::iterator<std::input_iterator_tag,
            std::pair<const TKey, TValue>> {
    public:
        bool operator==(const const_iterator & other) const {
            return it == other.it;
        }

        bool operator!=(const const_iterator & other) const {
            return !(*this == other);
        }

        const std::pair<const TKey, TValue> & operator*() const {
            return const_cast<const std::pair<const TKey, TValue> &>(it.dereference());
        }

        const std::pair<const TKey, TValue> & operator->() const {
            return **this;
        };

        const_iterator & operator++() {
            ++it;
            return *this;
        }

        const_iterator & operator++(int) {
            const_iterator res = *this;
            ++*this;

            return res;
        }

    private:
        const_iterator(Node * node)
                : it(node) {
        }

        iterator it = iterator(nullptr);

        friend class EvictingCacheMap;
    };

    /**
     * Construct a EvictingCacheMap
     * @param capacity maximum size of the cache map.  Once the map size exceeds
     *    maxSize, the map will begin to evict.
    */
    explicit EvictingCacheMap(std::size_t capacity)
            : capacity(capacity) {
    }

    EvictingCacheMap(const EvictingCacheMap & other) {
        *this = other;
    }

    EvictingCacheMap & operator=(const EvictingCacheMap & other) {
        if (this == &other)
            return *this;

        capacity = other.capacity;

        hashTable.clear();
        for (auto && [k, v] : other) {
            put(k, v);
        }

        return *this;
    }

    EvictingCacheMap(EvictingCacheMap && other) {
        *this = std::move(other);
    }

    EvictingCacheMap & operator=(EvictingCacheMap && other) {
        if (this == &other)
            return *this;

        capacity = other.capacity;
        hashTable = std::move(other.hashTable);
        tail = other.tail;
        head = other.head;

        return *this;
    }

    ~EvictingCacheMap() {
    }

    /**
     * Check for existence of a specific key in the map.  This operation has
     *     no effect on LRU order.
     * @param key key to search for
     * @return true if exists, false otherwise
    */
    bool exists(const TKey & key) const {
        return false;
    }

    /**
     * Get the value associated with a specific key.  This function always
     *     promotes a found value to the head of the LRU.
     * @param key key associated with the value
     * @return the value if it exists
    */
    std::optional<TValue> get(const TKey & key) {
        return {};
    }

    /**
     * Get the iterator associated with a specific key.  This function always
     *     promotes a found value to the head of the LRU.
     * @param key key to associate with value
     * @return the iterator of the object (a std::pair of const TKey, TValue) or
     *     end() if it does not exist
    */
    iterator find(const TKey & key) {
        return iterator(nullptr);
    }

    /**
     * Erase the key-value pair associated with key if it exists.
     * @param key key associated with the value
     * @return true if the key existed and was erased, else false
    */
    bool erase(const TKey & key) {
        return false;
    }

    /**
     * Set a key-value pair in the dictionary
     * @param key key to associate with value
     * @param value value to associate with the key
     */
    template <class T, class E>
    void put(const T && key, const E && value) {

    }

    /**
     * Get the number of elements in the dictionary
     * @return the size of the dictionary
    */
    std::size_t size() const {
        return 0;
    }

    /**
     * Typical empty function
     * @return true if empty, false otherwise
    */
    bool empty() const {
        return false;
    }

    void clear() {

    }

    // Iterators and such
    iterator begin() noexcept {
        return iterator(head);
    }

    iterator end() noexcept {
        return iterator(nullptr);
    }

    const_iterator begin() const noexcept {
        return const_iterator(head);
    }

    const_iterator end() const noexcept {
        return const_iterator(nullptr);
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }

private:
    struct Node {
        Node() = delete;

        Node(TKey && key, TValue && value)
                : data(std::move(key), std::move(value)) {
        }

        std::pair<const TKey, TValue> data;

        Node * prev = nullptr;
        Node * next = nullptr;
    };

    std::size_t capacity;

    std::vector<std::list<Node>> hashTable;

    Node * tail = nullptr;
    Node * head = nullptr;
};

template <class TKey, class TValue, class THash>
const std::string EvictingCacheMap<TKey, TValue, THash>::iterator::OUT_OF_RANGE
        = "Iterator is out of range";

#endif //LRU_EVICTINGCACHEMAP_H
