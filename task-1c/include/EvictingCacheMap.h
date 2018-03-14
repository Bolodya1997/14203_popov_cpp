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
private:
    struct Node;
    using NodePtr = typename std::list<Node>::iterator;

    template <class It, class V>
    class BaseIterator;

public:
    using value_type = std::pair<const TKey, TValue>;

    class iterator :
            public BaseIterator<iterator, std::pair<const TKey, TValue>> {
    public:
        iterator() = default;

        iterator(const iterator & other) = default;

        ~iterator() override = default;

    private:
        explicit iterator(const NodePtr & node);

        friend class EvictingCacheMap<TKey, TValue, THash>;
    };

    class const_iterator :
            public BaseIterator<const_iterator, const std::pair<const TKey, TValue>> {
    public:
        const_iterator() = default;

        const_iterator(const const_iterator & other) = default;

        ~const_iterator() override = default;

    private:
        explicit const_iterator(const NodePtr & node);

        friend class EvictingCacheMap<TKey, TValue, THash>;
    };

    /**
     * Construct a EvictingCacheMap
     * @param capacity maximum size of the cache map.  Once the map size exceeds
     *    maxSize, the map will begin to evict.
    */
    explicit EvictingCacheMap(std::size_t capacity);

    EvictingCacheMap(const EvictingCacheMap & other);
    EvictingCacheMap(EvictingCacheMap && other);

    ~EvictingCacheMap() = default;

    EvictingCacheMap & operator=(const EvictingCacheMap & other);
    EvictingCacheMap & operator=(EvictingCacheMap && other);

    /**
     * Check for existence of a specific key in the map.  This operation has
     *     no effect on LRU order.
     * @param key key to search for
     * @return true if exists, false otherwise
    */
    bool exists(const TKey & key) const;

    /**
     * Get the value associated with a specific key.  This function always
     *     promotes a found value to the head of the LRU.
     * @param key key associated with the value
     * @return the value if it exists
    */
    std::optional<TValue> get(const TKey & key);

    /**
     * Get the iterator associated with a specific key.  This function always
     *     promotes a found value to the head of the LRU.
     * @param key key to associate with value
     * @return the iterator of the object (a std::pair of const TKey, TValue) or
     *     end() if it does not exist
    */
    iterator find(const TKey & key);

    /**
     * Erase the key-value pair associated with key if it exists.
     * @param key key associated with the value
     * @return true if the key existed and was erased, else false
    */
    bool erase(const TKey & key);

    /**
     * Set a key-value pair in the dictionary
     * @param key key to associate with value
     * @param value value to associate with the key
     */
    template <class T, class E>
    void put(T && key, E && value);

    /**
     * Get the number of elements in the dictionary
     * @return the size of the dictionary
    */
    std::size_t size() const;

    /**
     * Typical empty function
     * @return true if empty, false otherwise
    */
    bool empty() const;

    void clear();

    // Iterators and such
    iterator begin() noexcept;
    iterator end() noexcept;

    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;

    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;

private:
    struct Node {
        Node() = delete;
        template <class T, class E>
        explicit Node(T && key, E && value);

        Node(const Node & other) = delete;
        Node(Node && other) noexcept = delete;

        std::pair<const TKey, TValue> data;

        NodePtr prev = NULL_NODE;
        NodePtr next = NULL_NODE;
    };

    template <class It, class V>
    class BaseIterator {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = V;
        using pointer = value_type *;
        using reference = value_type &;
        using iterator_category = std::forward_iterator_tag;

        BaseIterator();
        virtual ~BaseIterator() = default;

        explicit BaseIterator(const It & other);

        BaseIterator & operator=(const It & other);

        bool operator==(const It & other) const noexcept;
        bool operator!=(const It & other) const noexcept;

        reference operator*() const;
        pointer operator->() const;

        It & operator++();
        It operator++(int);

    protected:
        explicit BaseIterator(const NodePtr & node);

    private:
        static constexpr const char * OUT_OF_RANGE = "Iterator is out of range";

        NodePtr node;
    };

    static const NodePtr NULL_NODE;

    static constexpr const std::size_t LIST_SIZE = 10;

    std::vector<std::list<Node>> hashTable;

    std::size_t capacity;
    std::size_t _size = 0;

    NodePtr tail = NULL_NODE;
    NodePtr head = NULL_NODE;

    std::size_t keyToPos(const TKey & key) const noexcept;

    const NodePtr findNode(const TKey & key) const;
    void promoteToHead(const NodePtr & node);

    void removeNode(const NodePtr & node) noexcept;
    void addNode(const NodePtr & node) noexcept;
};

#include "EvictingCacheMap.inc"

#endif //LRU_EVICTINGCACHEMAP_H
