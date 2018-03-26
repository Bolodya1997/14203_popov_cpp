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
        using Type = typename SAccessor::Type;

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

        bool operator==(const Accessor & other) const {
            return sAccessor == other.sAccessor;
        }

        bool operator!=(const Accessor & other) const {
            return sAccessor != other.sAccessor;
        }

        bool hasValue() {
            return n == 0 && sAccessor.hasValue();
        }

        Type operator*() {
            return *sAccessor;
        }

        Accessor & operator++() {
            ++sAccessor;
            --n;

            return *this;
        }

    private:
        SAccessor sAccessor;
        SAccessor end;

        std::size_t n;
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
