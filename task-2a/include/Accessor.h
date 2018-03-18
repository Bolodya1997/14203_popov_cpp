#ifndef STREAM_ACCESSOR_H
#define STREAM_ACCESSOR_H

#include <utility>

template <class SAccessor, class Modifier>
class Accessor {
public:
    using value_type = typename Modifier::Out;

    Accessor() = default;
    explicit Accessor(const SAccessor & sAccessor, const Modifier & modifier)
            : sAccessor(sAccessor),
              modifier(modifier) {
    }

    Accessor(const Accessor & other) = default;
    Accessor(Accessor && other) noexcept = default;

    ~Accessor() = default;

    Accessor & operator=(const Accessor & other) = default;
    Accessor & operator=(Accessor && oher) noexcept = default;

    bool operator==(const Accessor & other) {
        return sAccessor == other.sAccessor;
    }

    bool operator!=(const Accessor & other) {
        return sAccessor != other.sAccessor;
    }

    value_type operator*() const {
        return modifier.modify(*sAccessor);
    }

    Accessor & operator++() {
        ++sAccessor;

        return *this;
    }

private:
    SAccessor sAccessor;

    Modifier modifier;
};

#endif //STREAM_ACCESSOR_H
