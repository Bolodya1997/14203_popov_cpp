#ifndef STREAM_FILTERACCESSOR_H
#define STREAM_FILTERACCESSOR_H

#include <iterator>

#include "Utils.h"

template <class Accessor, class Predicate>
class FilterAccessor {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = typename std::iterator_traits<Accessor>::value_type;
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::forward_iterator_tag;

    FilterAccessor() = delete;
    FilterAccessor(const Accessor & _accessor, const Accessor & _end,
                   const Predicate & _predicate)
            : accessor(_accessor),
              end(_end),
              predicate(_predicate) {
    }

    FilterAccessor(const FilterAccessor &) = default;
    FilterAccessor(FilterAccessor &&) noexcept = default;

    ~FilterAccessor() = default;

    FilterAccessor & operator=(const FilterAccessor &) = delete;
    FilterAccessor & operator=(FilterAccessor &&) noexcept = delete;

    bool operator==(const FilterAccessor & other) {
        skip();

        return accessor == other.accessor;
    }

    bool operator!=(const FilterAccessor & other) {
        skip();

        return accessor != other.accessor;
    }

    value_type operator*() {
        skip();

        if (hasValue)
            return value;
        return *accessor;   //  accessor == end
    }

    FilterAccessor & operator++() {
        skip();

        ++accessor;
        hasValue = false;

        return *this;
    }

private:
    Accessor accessor;
    Accessor end;

    const Predicate predicate;

    bool hasValue = false;
    value_type value;

    void skip() {
        if (hasValue || accessor == end)
            return;

        for (; accessor != end; ++accessor) {
            value = *accessor;
            if (predicate(value))
                break;
        }

        hasValue = accessor != end;
    }
};

#endif //STREAM_FILTERACCESSOR_H
