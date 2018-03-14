#include "EvictingCacheMap.h"

//  EvictingCacheMap::iterator

template <class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::iterator::
iterator(const EvictingCacheMap::NodePtr & node)
        : BaseIterator<iterator, std::pair<const TKey, TValue>>(node) {
}

//  EvictingCacheMap::const_iterator

template <class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::const_iterator::
const_iterator(const EvictingCacheMap::NodePtr & node)
        : BaseIterator<const_iterator, const std::pair<const TKey, TValue>>(node) {
}

//  EvictingCacheMap (public)

template <class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::
EvictingCacheMap(std::size_t capacity)
        : hashTable(capacity), capacity(capacity) {
}

template <class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::
EvictingCacheMap(const EvictingCacheMap & other) {
    *this = other;
}

template <class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::
EvictingCacheMap(EvictingCacheMap && other) {
    *this = std::move(other);
}

template <class TKey, class TValue, class THash>
class EvictingCacheMap<TKey, TValue, THash> &
EvictingCacheMap<TKey, TValue, THash>::
operator=(const EvictingCacheMap & other) {
    if (this == &other)
        return *this;

    capacity = other.capacity;

    clear();
    for (auto node = other.tail; node != NULL_NODE; node = node->prev) {
        put(node->data.first, node->data.second);
    }

    return *this;
}

template <class TKey, class TValue, class THash>
class EvictingCacheMap<TKey, TValue, THash> &
EvictingCacheMap<TKey, TValue, THash>::
operator=(EvictingCacheMap && other) {
    if (this == &other)
        return *this;

    hashTable = std::move(other.hashTable);

    capacity = other.capacity;
    _size = other._size;
    tail = other.tail;
    head = other.head;

    return *this;
}

template <class TKey, class TValue, class THash>
bool
EvictingCacheMap<TKey, TValue, THash>::
exists(const TKey & key) const {
    return findNode(key) != NULL_NODE;
}

template <class TKey, class TValue, class THash>
std::optional<TValue>
EvictingCacheMap<TKey, TValue, THash>::
get(const TKey & key) {
    auto node = findNode(key);
    if (node == NULL_NODE)
        return {};

    promoteToHead(node);

    return node->data.second;
}

template <class TKey, class TValue, class THash>
class EvictingCacheMap<TKey, TValue, THash>::iterator
EvictingCacheMap<TKey, TValue, THash>::
find(const TKey & key) {
    auto node = findNode(key);
    if (node == NULL_NODE)
        return end();

    promoteToHead(node);

    return iterator(node);
}

template <class TKey, class TValue, class THash>
bool
EvictingCacheMap<TKey, TValue, THash>::
erase(const TKey & key) {
    if (capacity == 0)
        return false;

    auto & list = hashTable[keyToPos(key)];
    for (auto node = list.begin(); node != list.end(); ++node) {
        if (node->data.first != key)
            continue;

        removeNode(node);
        list.erase(node);

        --_size;

        return true;
    }

    return false;
}

template <class TKey, class TValue, class THash>
template <class T, class E>
void
EvictingCacheMap<TKey, TValue, THash>::
put(T && key, E && value) {
    if (capacity == 0)
        return;

    auto node = findNode(key);
    if (node != NULL_NODE) {
        node->data.second = std::forward<E>(value);
        promoteToHead(node);

        return;
    }

    auto & list = hashTable[keyToPos(key)];
    list.emplace_front(std::forward<T>(key), std::forward<E>(value));
    addNode(list.begin());

    if (_size == capacity)
        erase(tail->data.first);
    ++_size;
}

template <class TKey, class TValue, class THash>
std::size_t
EvictingCacheMap<TKey, TValue, THash>::
size() const {
    return _size;
}

template <class TKey, class TValue, class THash>
bool
EvictingCacheMap<TKey, TValue, THash>::
empty() const {
    return _size == 0;
}

template <class TKey, class TValue, class THash>
void
EvictingCacheMap<TKey, TValue, THash>::
clear() {
    hashTable = std::vector<std::list<Node>>(capacity);

    _size = 0;

    tail = NULL_NODE;
    head = NULL_NODE;
}

template <class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::iterator
EvictingCacheMap<TKey, TValue, THash>::
begin() noexcept {
    return iterator(head);
}

template <class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::iterator
EvictingCacheMap<TKey, TValue, THash>::
end() noexcept {
    return iterator(NULL_NODE);
}

template <class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::const_iterator
EvictingCacheMap<TKey, TValue, THash>::
begin() const noexcept {
    return const_iterator(head);
}

template <class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::const_iterator
EvictingCacheMap<TKey, TValue, THash>::
end() const noexcept {
    return const_iterator(NULL_NODE);
}

template <class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::const_iterator
EvictingCacheMap<TKey, TValue, THash>::
cbegin() const noexcept {
    return begin();
}

template <class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::const_iterator
EvictingCacheMap<TKey, TValue, THash>::
cend() const noexcept {
    return end();
}

//  EvictingCacheMap::Node

template <class TKey, class TValue, class THash>
template <class T, class E>
EvictingCacheMap<TKey, TValue, THash>::Node::
Node(T && key, E && value)
        : data(std::forward<T>(key), std::forward<E>(value)) {
}

//  EvictingCacheMap::BaseIterator

template <class TKey, class TValue, class THash>
template <class It, class V>
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
BaseIterator()
        : node(nullptr) {
}

template <class TKey, class TValue, class THash>
template <class It, class V>
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
BaseIterator(const It & other) {
    *this = other;
}

template <class TKey, class TValue, class THash>
template <class It, class V>
class EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V> &
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
operator=(const It & other) {
    if (this != &other) {
        node = other.node;
    }

    return *this;
}

template <class TKey, class TValue, class THash>
template <class It, class V>
bool
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
operator==(const It & other) const noexcept {
    return node == other.node;
}

template <class TKey, class TValue, class THash>
template <class It, class V>
bool
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
operator!=(const It & other) const noexcept {
    return !(*this == other);
}

template <class TKey, class TValue, class THash>
template <class It, class V>
typename EvictingCacheMap<TKey, TValue, THash>::template BaseIterator<It, V>::reference
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
operator*() const {
    if (node == NULL_NODE)
        throw std::out_of_range(OUT_OF_RANGE);
    return node->data;
}

template <class TKey, class TValue, class THash>
template <class It, class V>
typename EvictingCacheMap<TKey, TValue, THash>::template BaseIterator<It, V>::pointer
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
operator->() const {
    return &this->operator*();
}

template <class TKey, class TValue, class THash>
template <class It, class V>
It &
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
operator++() {
    if (node == NULL_NODE)
        throw std::out_of_range(OUT_OF_RANGE);
    node = node->next;

    return static_cast<It &>(*this);
}

template <class TKey, class TValue, class THash>
template <class It, class V>
It
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
operator++(int) {
    It res = static_cast<It &>(*this);
    ++*this;

    return res;
}

template <class TKey, class TValue, class THash>
template <class It, class V>
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
BaseIterator(const NodePtr & node)
        : node(node) {
}

//  EvictingCacheMap (private)

template <class TKey, class TValue, class THash>
const typename EvictingCacheMap<TKey, TValue, THash>::NodePtr
EvictingCacheMap<TKey, TValue, THash>::NULL_NODE
        = EvictingCacheMap<TKey, TValue, THash>::NodePtr();

template <class TKey, class TValue, class THash>
inline std::size_t
EvictingCacheMap<TKey, TValue, THash>::
keyToPos(const TKey & key) const noexcept {
    double multiplier = static_cast<double>(capacity) / std::numeric_limits<std::size_t>::max();
    std::size_t pos = static_cast<std::size_t>(THash()(key) * multiplier);

    return (pos < capacity) ? pos : capacity - 1;
}

template <class TKey, class TValue, class THash>
const typename EvictingCacheMap<TKey, TValue, THash>::NodePtr
EvictingCacheMap<TKey, TValue, THash>::
findNode(const TKey & key) const {
    if (capacity == 0)
        return NULL_NODE;

    auto & list = const_cast<std::list<Node> &>(hashTable[keyToPos(key)]);
    for (auto node = list.begin(); node != list.end(); ++node) {
        if (node->data.first == key)
            return node;
    }

    return NULL_NODE;
}

template <class TKey, class TValue, class THash>
void
EvictingCacheMap<TKey, TValue, THash>::
promoteToHead(const NodePtr & node) {
    if (node == head)
        return;

    if (node->prev != NULL_NODE)
        node->prev->next = node->next;
    if (node->next != NULL_NODE)
        node->next->prev = node->prev;

    node->prev = NULL_NODE;
    node->next = head;

    head->prev = node;  //  head != nullptr (map is not empty)
    head = node;
}

template <class TKey, class TValue, class THash>
void
EvictingCacheMap<TKey, TValue, THash>::
removeNode(const NodePtr & node) noexcept {
    if (node->prev != NULL_NODE)
        node->prev->next = node->next;
    if (node->next != NULL_NODE)
        node->next->prev = node->prev;

    if (node == head)
        head = node->next;
    if (node == tail)
        tail = node->prev;
}

template <class TKey, class TValue, class THash>
void
EvictingCacheMap<TKey, TValue, THash>::
addNode(const NodePtr & node) noexcept {
    node->prev = NULL_NODE;
    node->next = head;

    if (head != NULL_NODE)
        head->prev = node;
    head = node;

    if (tail == NULL_NODE)  //  empty map
        tail = node;
}