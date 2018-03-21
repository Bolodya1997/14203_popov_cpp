#ifndef STREAM_RANGEACCESSOR_H
#define STREAM_RANGEACCESSOR_H

#include <iterator>

template <class Accessor>
class RangeAccessor {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = typename std::iterator_traits<Accessor>::value_type;
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::forward_iterator_tag;

    RangeAccessor() = delete;
    RangeAccessor(const Accessor & _accessor, std::size_t _pos)
            : accessor(_accessor),
              pos(_pos) {
    }

    RangeAccessor(const RangeAccessor &) = default;
    RangeAccessor(RangeAccessor &&) noexcept = default;

    ~RangeAccessor() = default;

    RangeAccessor & operator=(const RangeAccessor &) = delete;
    RangeAccessor & operator=(RangeAccessor &&) noexcept = delete;

    bool operator==(const RangeAccessor & other) {
        return pos == other.pos || accessor == other.accessor;
    }

    bool operator!=(const RangeAccessor & other) {
        return pos != other.pos && accessor != other.accessor;
    }

    value_type operator*() {
        return *accessor;
    }

    RangeAccessor & operator++() {
        ++pos;
        ++accessor;

        return *this;
    }

private:
    Accessor accessor;

    std::size_t pos = 0;
};

#endif //STREAM_RANGEACCESSOR_H
