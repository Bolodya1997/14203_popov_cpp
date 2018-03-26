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
        using Type = typename SAccessor::Type;

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

        bool operator!=(const Accessor & other) const {
            return pos != other.pos && sAccessor != other.sAccessor;
        }

        bool hasValue() {
            return sAccessor.hasValue();
        }

        Type operator*() {
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
