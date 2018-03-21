#ifndef STREAM_GENACCESSOR_H
#define STREAM_GENACCESSOR_H

#include <iterator>

template <class Generator>
class GenAccessor {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = decltype(std::declval<Generator>()());
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::forward_iterator_tag;

    GenAccessor() = delete;
    explicit GenAccessor(const Generator & generator)
            : generator(generator) {
    }

    GenAccessor(const GenAccessor &) = default;
    GenAccessor(GenAccessor &&) noexcept = default;

    ~GenAccessor() = default;

    GenAccessor & operator=(const GenAccessor &) = default;
    GenAccessor & operator=(GenAccessor &&) noexcept = delete;

    bool operator==(const GenAccessor & other) {
        return false;
    }

    bool operator!=(const GenAccessor & other) {
        return true;
    }

    value_type operator*() {
        for (; toInvoke > 1; --toInvoke) {
            generator();
        }
        toInvoke = 0;

        return generator();
    }

    GenAccessor & operator++() {
        ++toInvoke;

        return *this;
    }

private:
    const Generator generator;

    std::size_t toInvoke = 1;
};

#endif //STREAM_GENACCESSOR_H
