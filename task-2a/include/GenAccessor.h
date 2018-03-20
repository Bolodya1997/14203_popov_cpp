#ifndef STREAM_GENACCESSOR_H
#define STREAM_GENACCESSOR_H

#include <utility>

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

    GenAccessor & operator=(const GenAccessor &) = delete;
    GenAccessor & operator=(GenAccessor &&) noexcept = delete;

    bool operator==(const GenAccessor & other) {
        return false;
    }

    bool operator!=(const GenAccessor & other) {
        return true;
    }

    value_type operator*() const {
        if (!invoked)
            const_cast<GenAccessor *>(this)->invoked = true;

        return generator();
    }

    GenAccessor & operator++() {
        if (!invoked) {
            generator();
            invoked = false;
        }

        return *this;
    }

private:
    const Generator & generator;

    bool invoked = false;
};

#endif //STREAM_GENACCESSOR_H
