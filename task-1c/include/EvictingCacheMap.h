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
private:
    struct Node;

public:
    class iterator: public std::iterator<std::input_iterator_tag,
            std::pair<const TKey, TValue>> {
    public:
        bool operator==(const iterator & other) const noexcept;
        bool operator!=(const iterator & other) const noexcept;

        std::pair<const TKey, TValue> & operator*();
        std::pair<const TKey, TValue> * operator->();

        iterator & operator++();
        iterator operator++(int);

    private:
        static constexpr const char *OUT_OF_RANGE = "Iterator is out of range";

        iterator(Node * node);

        Node * node = nullptr;

        std::pair<const TKey, TValue> & dereference() const;

        friend class const_iterator;
        friend class EvictingCacheMap<TKey, TValue, THash>;
    };

    class const_iterator: public std::iterator<std::input_iterator_tag,
            std::pair<const TKey, TValue>> {
    public:
        bool operator==(const const_iterator & other) const noexcept;
        bool operator!=(const const_iterator & other) const noexcept;

        const std::pair<const TKey, TValue> & operator*() const;
        const std::pair<const TKey, TValue> * operator->() const;;

        const_iterator & operator++();
        const_iterator operator++(int);

    private:
        const_iterator(Node * node);

        EvictingCacheMap<TKey, TValue, THash>::iterator it
                = EvictingCacheMap<TKey, TValue, THash>::iterator(nullptr);

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

    ~EvictingCacheMap();

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
        Node(const Node & other) = delete;
        explicit Node(TKey && key, TValue && value);

        std::pair<const TKey, TValue> data;

        Node * prev = nullptr;
        Node * next = nullptr;
    };

    std::size_t capacity;

    std::vector<std::list<Node>> hashTable;

    Node * tail = nullptr;
    Node * head = nullptr;
};

//  EvictingCacheMap::iterator

template<class TKey, class TValue, class THash>
inline bool
EvictingCacheMap<TKey, TValue, THash>::iterator::
operator==(const EvictingCacheMap::iterator & other) const noexcept {
    return node == other.node;
}

template<class TKey, class TValue, class THash>
inline bool
EvictingCacheMap<TKey, TValue, THash>::iterator::
operator!=(const EvictingCacheMap::iterator & other) const noexcept {
    return !(*this == other);
}

template<class TKey, class TValue, class THash>
inline std::pair<const TKey, TValue> &
EvictingCacheMap<TKey, TValue, THash>::iterator::
operator*() {
    return dereference();
}

template<class TKey, class TValue, class THash>
inline std::pair<const TKey, TValue> *
EvictingCacheMap<TKey, TValue, THash>::iterator::
operator->() {
    return &dereference();
}

template<class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::iterator &
EvictingCacheMap<TKey, TValue, THash>::iterator::
operator++() {
    if (node == nullptr)
        throw std::out_of_range(OUT_OF_RANGE);
    node = node->next;

    return *this;
}

template<class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::iterator
EvictingCacheMap<TKey, TValue, THash>::iterator::
operator++(int) {
    iterator res = *this;
    ++*this;

    return res;
}

template<class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::iterator::
iterator(EvictingCacheMap::Node * node)
        : node(node) {
}

template<class TKey, class TValue, class THash>
inline std::pair<const TKey, TValue> &
EvictingCacheMap<TKey, TValue, THash>::iterator::
dereference() const {
    if (node == nullptr)
        throw std::out_of_range(OUT_OF_RANGE);
    return node->data;
}

//  EvictingCacheMap::const_iterator

template<class TKey, class TValue, class THash>
inline bool
EvictingCacheMap<TKey, TValue, THash>::const_iterator::
operator==(const EvictingCacheMap::const_iterator & other) const noexcept {
    return it == other.it;
}

template<class TKey, class TValue, class THash>
inline bool
EvictingCacheMap<TKey, TValue, THash>::const_iterator::
operator!=(const EvictingCacheMap::const_iterator & other) const noexcept {
    return !(*this == other);
}

template<class TKey, class TValue, class THash>
inline const std::pair<const TKey, TValue> &
EvictingCacheMap<TKey, TValue, THash>::const_iterator::operator*() const {
    return const_cast<const std::pair<const TKey, TValue> &>(it.dereference());
}

template<class TKey, class TValue, class THash>
inline const std::pair<const TKey, TValue> *
EvictingCacheMap<TKey, TValue, THash>::const_iterator::operator->() const {
    return &it.dereference();
}

template<class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::const_iterator &
EvictingCacheMap<TKey, TValue, THash>::const_iterator::operator++() {
    ++it;
    return *this;
}

template<class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::const_iterator
EvictingCacheMap<TKey, TValue, THash>::const_iterator::operator++(int) {
    const_iterator res = *this;
    ++*this;

    return res;
}

template<class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::
const_iterator::const_iterator(EvictingCacheMap::Node * node)
        : it(node) {
}

//  EvictingCacheMap

template<class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::
EvictingCacheMap(std::size_t capacity)
        : capacity(capacity) {
}

template<class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::
EvictingCacheMap(const EvictingCacheMap & other) {
    *this = other;
}

template<class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::
EvictingCacheMap(EvictingCacheMap && other) {
    *this = std::move(other);
}

template<class TKey, class TValue, class THash>
class EvictingCacheMap<TKey, TValue, THash> &
EvictingCacheMap<TKey, TValue, THash>::
operator=(const EvictingCacheMap & other) {
    if (this == &other)
        return *this;

    capacity = other.capacity;

    hashTable.clear();
    for (auto && [k, v] : other) {
        put(k, v);
    }

    return *this;
}

template<class TKey, class TValue, class THash>
class EvictingCacheMap<TKey, TValue, THash> &
EvictingCacheMap<TKey, TValue, THash>::
operator=(EvictingCacheMap && other) {
    if (this == &other)
        return *this;

    capacity = other.capacity;
    hashTable = std::move(other.hashTable);
    tail = other.tail;
    head = other.head;

    return *this;
}

template<class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::
~EvictingCacheMap() {
}

template<class TKey, class TValue, class THash>
bool
EvictingCacheMap<TKey, TValue, THash>::
exists(const TKey & key) const {
    return false;
}

template<class TKey, class TValue, class THash>
std::optional<TValue>
EvictingCacheMap<TKey, TValue, THash>::
get(const TKey & key) {
    return {};
}

template<class TKey, class TValue, class THash>
class EvictingCacheMap<TKey, TValue, THash>::iterator
EvictingCacheMap<TKey, TValue, THash>::
find(const TKey & key) {
    return EvictingCacheMap<TKey, TValue, THash>::iterator(nullptr);
}

template<class TKey, class TValue, class THash>
bool
EvictingCacheMap<TKey, TValue, THash>::
erase(const TKey & key) {
    return false;
}

template<class TKey, class TValue, class THash>
template<class T, class E>
void
EvictingCacheMap<TKey, TValue, THash>::
put(T && key, E && value) {

}

template<class TKey, class TValue, class THash>
std::size_t
EvictingCacheMap<TKey, TValue, THash>::
size() const {
    return 0;
}

template<class TKey, class TValue, class THash>
bool
EvictingCacheMap<TKey, TValue, THash>::
empty() const {
    return false;
}

template<class TKey, class TValue, class THash>
void
EvictingCacheMap<TKey, TValue, THash>::
clear() {

}

template<class TKey, class TValue, class THash>
class EvictingCacheMap<TKey, TValue, THash>::iterator
EvictingCacheMap<TKey, TValue, THash>::
begin() noexcept {
    return iterator(head);
}

template<class TKey, class TValue, class THash>
class EvictingCacheMap<TKey, TValue, THash>::iterator
EvictingCacheMap<TKey, TValue, THash>::
end() noexcept {
    return iterator(nullptr);
}

template<class TKey, class TValue, class THash>
class EvictingCacheMap<TKey, TValue, THash>::const_iterator
EvictingCacheMap<TKey, TValue, THash>::
begin() const noexcept {
    return const_iterator(head);
}

template<class TKey, class TValue, class THash>
class EvictingCacheMap<TKey, TValue, THash>::const_iterator
EvictingCacheMap<TKey, TValue, THash>::
end() const noexcept {
    return const_iterator(nullptr);
}

template<class TKey, class TValue, class THash>
class EvictingCacheMap<TKey, TValue, THash>::const_iterator
EvictingCacheMap<TKey, TValue, THash>::
cbegin() const noexcept {
    return begin();
}

template<class TKey, class TValue, class THash>
class EvictingCacheMap<TKey, TValue, THash>::const_iterator
EvictingCacheMap<TKey, TValue, THash>::
cend() const noexcept {
    return end();
}

//  EvictingCacheMap::Node

template<class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::Node::
Node(TKey && key, TValue && value)
        : data(std::move(key), std::move(value)) {
}

#endif //LRU_EVICTINGCACHEMAP_H
