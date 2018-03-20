#ifndef STREAM_RANGEACCESSOR_H
#define STREAM_RANGEACCESSOR_H

#include <iterator>

template <class SAccessor>
class RangeAccessor {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = typename std::iterator_traits<SAccessor>::value_type;
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::forward_iterator_tag;

    RangeAccessor() = delete;
    RangeAccessor(const SAccessor & _sAccessor, std::size_t _pos)
            : sAccessor(_sAccessor),
              pos(_pos) {
    }

    RangeAccessor(const RangeAccessor & other) = default;
    RangeAccessor(RangeAccessor && other) noexcept = default;

    ~RangeAccessor() = default;

    RangeAccessor & operator=(const RangeAccessor & other) = default;
    RangeAccessor & operator=(RangeAccessor && oher) noexcept = default;

    bool operator==(const RangeAccessor & other) {
        return pos == other.pos || sAccessor == other.sAccessor;
    }

    bool operator!=(const RangeAccessor & other) {
        return pos != other.pos && sAccessor != other.sAccessor;
    }

    value_type operator*() const {
        return *sAccessor;
    }

    RangeAccessor & operator++() {
        ++pos;
        ++sAccessor;

        return *this;
    }

private:
    SAccessor sAccessor;

    std::size_t pos = 0;
};

#endif //STREAM_RANGEACCESSOR_H
