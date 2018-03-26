#ifndef STREAM_GENACCESSOR_H
#define STREAM_GENACCESSOR_H

#include <iterator>

template <class Generator>
class GenAccessor {
public:
    using Type = decltype(std::declval<Generator>()());

    GenAccessor() = delete;
    explicit GenAccessor(const Generator & generator)
            : generator(generator) {
    }

    GenAccessor(const GenAccessor &) = default;
    GenAccessor(GenAccessor &&) noexcept = default;

    ~GenAccessor() = default;

    GenAccessor & operator=(const GenAccessor &) = delete;
    GenAccessor & operator=(GenAccessor &&) noexcept = delete;

    bool operator!=(const GenAccessor & other) const {
        return true;
    }

    bool hasValue() const {
        return true;
    }

    Type operator*() {
        if (toInvoke == 0)
            throw std::runtime_error("double dereferencing GenAccessor");

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
    const Generator & generator;

    std::size_t toInvoke = 1;
};

#endif //STREAM_GENACCESSOR_H
