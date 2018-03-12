#include "EvictingCacheMap.h"

//  EvictingCacheMap::iterator

template<class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::iterator::
iterator(EvictingCacheMap::Node * node)
        : BaseIterator<iterator, std::pair<const TKey, TValue>>(node) {
}

//  EvictingCacheMap::const_iterator

template<class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::const_iterator::
const_iterator(EvictingCacheMap::Node * node)
        : BaseIterator<const_iterator, const std::pair<const TKey, TValue>>(node) {
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
    for (auto & [k, v] : other) {
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
bool
EvictingCacheMap<TKey, TValue, THash>::
exists(const TKey & key) const {
    return findNode(key) == nullptr;
}

template<class TKey, class TValue, class THash>
std::optional<TValue>
EvictingCacheMap<TKey, TValue, THash>::
get(const TKey & key) {
    Node * node = const_cast<Node *>(findNode(key));
    if (node == nullptr)
        return {};

    promoteToHead(node);

    return node->data.second;
}

template<class TKey, class TValue, class THash>
class EvictingCacheMap<TKey, TValue, THash>::iterator
EvictingCacheMap<TKey, TValue, THash>::
find(const TKey & key) {
    Node * node = const_cast<Node *>(findNode(key));
    if (node == nullptr)
        return end();

    promoteToHead(node);

    return iterator(node);
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
inline class EvictingCacheMap<TKey, TValue, THash>::iterator
EvictingCacheMap<TKey, TValue, THash>::
begin() noexcept {
    return iterator(head);
}

template<class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::iterator
EvictingCacheMap<TKey, TValue, THash>::
end() noexcept {
    return iterator(nullptr);
}

template<class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::const_iterator
EvictingCacheMap<TKey, TValue, THash>::
begin() const noexcept {
    return const_iterator(head);
}

template<class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::const_iterator
EvictingCacheMap<TKey, TValue, THash>::
end() const noexcept {
    return const_iterator(nullptr);
}

template<class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::const_iterator
EvictingCacheMap<TKey, TValue, THash>::
cbegin() const noexcept {
    return begin();
}

template<class TKey, class TValue, class THash>
inline class EvictingCacheMap<TKey, TValue, THash>::const_iterator
EvictingCacheMap<TKey, TValue, THash>::
cend() const noexcept {
    return end();
}

template<class TKey, class TValue, class THash>
inline size_t
EvictingCacheMap<TKey, TValue, THash>::
keyToPos(const TKey & key) const noexcept {
    double multiplier = static_cast<double>(capacity) / std::numeric_limits<size_t>::max();
    size_t pos = static_cast<size_t>(THash()(key) * multiplier);

    return (pos < capacity) ? pos : capacity - 1;
}

template<class TKey, class TValue, class THash>
const struct EvictingCacheMap<TKey, TValue, THash>::Node *
EvictingCacheMap<TKey, TValue, THash>::
findNode(const TKey & key) const {
    size_t pos = keyToPos(key);
    if (pos >= hashTable.size())
        return nullptr;

    auto & list = hashTable[pos];
    for (auto & node : list) {
        if (node.data.first == key)
            return &node;
    }

    return nullptr;
}

template<class TKey, class TValue, class THash>
void
EvictingCacheMap<TKey, TValue, THash>::
promoteToHead(EvictingCacheMap::Node * node) noexcept {
    if (node == head)
        return;

    if (node->prev != nullptr)
        node->prev->next = node->next;
    if (node->next != nullptr)
        node->next->prev = node->prev;

    node->prev = head;
    node->next = nullptr;

    head->next = node;  //  head != nullptr (map is not empty)
    head = node;
}

//  EvictingCacheMap::Node

template<class TKey, class TValue, class THash>
EvictingCacheMap<TKey, TValue, THash>::Node::
Node(TKey && key, TValue && value)
        : data(std::move(key), std::move(value)) {
}

//  EvictingCacheMap::BaseIterator

template<class TKey, class TValue, class THash>
template<class It, class V>
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
BaseIterator()
        : node(nullptr) {
}

template<class TKey, class TValue, class THash>
template<class It, class V>
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
BaseIterator(const It & other) {
    *this = other;
}

template<class TKey, class TValue, class THash>
template<class It, class V>
class EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V> &
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
operator=(const It & other) {
    if (this != &other) {
        node = other.node;
    }

    return *this;
}

template<class TKey, class TValue, class THash>
template<class It, class V>
bool
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
operator==(const It & other) const noexcept {
    return node == other.node;
}

template<class TKey, class TValue, class THash>
template<class It, class V>
bool
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
operator!=(const It & other) const noexcept {
    return !(*this == other);
}

template<class TKey, class TValue, class THash>
template<class It, class V>
typename EvictingCacheMap<TKey, TValue, THash>::template BaseIterator<It, V>::reference
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
operator*() const {
    if (node == nullptr)
        throw std::out_of_range(OUT_OF_RANGE);
    return node->data;
}

template<class TKey, class TValue, class THash>
template<class It, class V>
typename EvictingCacheMap<TKey, TValue, THash>::template BaseIterator<It, V>::pointer
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
operator->() const {
    return &this->operator*();
}

template<class TKey, class TValue, class THash>
template<class It, class V>
It &
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
operator++() {
    if (node == nullptr)
        throw std::out_of_range(OUT_OF_RANGE);
    node = node->next;

    return static_cast<It &>(*this);
}

template<class TKey, class TValue, class THash>
template<class It, class V>
It
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
operator++(int) {
    It res = static_cast<It &>(*this);
    ++*this;

    return res;
}

template<class TKey, class TValue, class THash>
template<class It, class V>
EvictingCacheMap<TKey, TValue, THash>::BaseIterator<It, V>::
BaseIterator(EvictingCacheMap::Node * node)
        : node(node) {
}
