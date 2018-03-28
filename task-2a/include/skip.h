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

        Accessor(const SAccessor & _sAccessor, std::size_t _n)
                : sAccessor(_sAccessor),
                  n(_n) {
        }

        Accessor(const Accessor &) = default;

        ~Accessor() = default;

        Accessor & operator=(const Accessor &) = delete;

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
            if (n > 0 && sAccessor.hasValue())
                --n;

            ++sAccessor;

            return *this;
        }

    private:
        SAccessor sAccessor;

        std::size_t n;
    };

    skip() = delete;

    explicit skip(const std::size_t _n)
            : n(_n) {
    }

    skip(const skip &) = default;
    skip(skip &&) noexcept = default;

    template <class SAccessor>
    auto modify(const SAccessor & begin, const SAccessor & end) const {
        return std::pair{ Accessor(begin, n),
                          Accessor(end, 0) };
    }

private:
    const std::size_t n;
};

#endif //STREAM_SKIP_H
