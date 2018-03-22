#ifndef STREAM_FILTER_H
#define STREAM_FILTER_H

#include <iterator>

#include "Utils.h"

template <class Predicate>
class filter {
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
                 const Predicate & _predicate)
                : sAccessor(_sAccessor),
                  end(_end),
                  predicate(_predicate) {
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

            if (hasValue)
                return value;
            return *sAccessor;   //  sAccessor == end
        }

        Accessor & operator++() {
            skip();

            ++sAccessor;
            hasValue = false;

            return *this;
        }

    private:
        SAccessor sAccessor;
        SAccessor end;

        const Predicate & predicate;

        bool hasValue = false;
        value_type value;

        void skip() {
            if (hasValue || sAccessor == end)
                return;

            for (; sAccessor != end; ++sAccessor) {
                value = *sAccessor;
                if (predicate(value))
                    break;
            }

            hasValue = sAccessor != end;
        }
    };

    filter() = delete;

    explicit filter(const Predicate & _predicate)
            : predicate(_predicate) {
    }

    explicit filter(Predicate && _predicate)
            : predicate(_predicate) {
    }

    template <class SAccessor>
    auto modify(SAccessor begin, SAccessor end) const {
        return std::pair{ Accessor<SAccessor>(begin, end, predicate),
                          Accessor<SAccessor>(end, end, predicate) };
    }

private:
    const Predicate predicate;
};

#endif //STREAM_FILTER_H
