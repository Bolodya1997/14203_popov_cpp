#ifndef STREAM_GET_H
#define STREAM_GET_H

#include <iterator>

#include "Utils.h"

class get {
public:
    using StreamTag = FiniteStreamTag;

    template <class SAccessor>
    class Accessor {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = typename std::iterator_traits<SAccessor>::value_type;
        using pointer = value_type *;
        using reference = value_type &;
        using iterator_category = std::forward_iterator_tag;

        Accessor() = delete;

        Accessor(const SAccessor & _sAccessor, std::size_t _pos)
                : sAccessor(_sAccessor),
                  pos(_pos) {
        }

        Accessor(const Accessor &) = default;
        Accessor(Accessor &&) noexcept = default;

        ~Accessor() = default;

        Accessor & operator=(const Accessor &) = delete;
        Accessor & operator=(Accessor &&) noexcept = delete;

        bool operator==(const Accessor & other) {
            return pos == other.pos || sAccessor == other.sAccessor;
        }

        bool operator!=(const Accessor & other) {
            return pos != other.pos && sAccessor != other.sAccessor;
        }

        value_type operator*() {
            return *sAccessor;
        }

        Accessor & operator++() {
            ++pos;
            ++sAccessor;

            return *this;
        }

    private:
        SAccessor sAccessor;

        std::size_t pos = 0;
    };

    get() = delete;

    explicit get(const std::size_t & _n)
            : n(_n) {
    }

    template <class SAccessor>
    auto modify(SAccessor begin, SAccessor end) const {
        return std::pair{ Accessor(begin, 0),
                          Accessor(end, n) };
    }

private:
    const std::size_t n;
};

#endif //STREAM_GET_H
