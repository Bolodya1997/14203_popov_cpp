#ifndef STREAM_GENACCESSOR_H
#define STREAM_GENACCESSOR_H

#include <utility>

template <class Generator>
class GenAccessor {
public:
    using value_type = decltype(std::declval<Generator>()());

    GenAccessor() = delete;
    explicit GenAccessor(const Generator & generator)
            : generator(generator) {
    }

    GenAccessor(const GenAccessor & other) = default;
    GenAccessor(GenAccessor && other) noexcept = default;

    ~GenAccessor() = default;

    GenAccessor & operator=(const GenAccessor & other) = delete;
    GenAccessor & operator=(GenAccessor && oher) noexcept = delete;

    bool operator==(const GenAccessor & other) {
        return false;
    }

    bool operator!=(const GenAccessor & other) {
        return true;
    }

    value_type operator*() const {
        return generator();
    }

    GenAccessor & operator++() {
        return *this;
    }

private:
    const Generator & generator;
};

#endif //STREAM_GENACCESSOR_H
