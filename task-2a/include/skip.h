#ifndef STREAM_SKIP_H
#define STREAM_SKIP_H

#include <iterator>

#include "Utils.h"

class skip {
public:
    using StreamTag = InfiniteStreamTag;

    template <class SAccessor>
    class Accessor {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = typename std::iterator_traits<SAccessor>::value_type;
        using pointer = value_type *;
        using reference = value_type &;
        using iterator_category = std::forward_iterator_tag;

        Accessor() = delete;
        Accessor(const SAccessor & _sAccessor, const SAccessor & _end,
                 std::size_t _n)
                : sAccessor(_sAccessor),
                  end(_end),
                  n(_n) {
        }

        Accessor(const Accessor &) = default;
        Accessor(Accessor &&) noexcept = default;

        ~Accessor() = default;

        Accessor & operator=(const Accessor &) = delete;
        Accessor & operator=(Accessor &&) noexcept = delete;

        bool operator==(const Accessor & other) {
            skip();

            return sAccessor == other.sAccessor;
        }

        bool operator!=(const Accessor & other) {
            skip();

            return sAccessor != other.sAccessor;
        }

        value_type operator*() {
            skip();

            return *sAccessor;
        }

        Accessor & operator++() {
            skip();

            ++sAccessor;

            return *this;
        }

    private:
        SAccessor sAccessor;
        SAccessor end;

        std::size_t n;

        void skip() {
            for (; n > 0 && sAccessor != end; --n) {
                ++sAccessor;
            }
        }
    };

    skip() = delete;
    explicit skip(const std::size_t _n)
            : n(_n) {
    }

    template <class SAccessor>
    auto modify(SAccessor begin, SAccessor end) const {
        return std::pair{ Accessor(begin, end, n),
                          Accessor(end, end, n) };
    }

private:
    const std::size_t n;
};

#endif //STREAM_SKIP_H
