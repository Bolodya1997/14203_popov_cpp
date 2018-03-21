#ifndef STREAM_SKIPACCESSOR_H
#define STREAM_SKIPACCESSOR_H

#include <iterator>

template <class Accessor>
class SkipAccessor {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = typename std::iterator_traits<Accessor>::value_type;
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::forward_iterator_tag;

    SkipAccessor() = delete;
    SkipAccessor(const Accessor & _accessor, const Accessor & _end,
                   std::size_t _n)
            : accessor(_accessor),
              end(_end),
              n(_n) {
    }

    SkipAccessor(const SkipAccessor &) = default;
    SkipAccessor(SkipAccessor &&) noexcept = default;

    ~SkipAccessor() = default;

    SkipAccessor & operator=(const SkipAccessor &) = delete;
    SkipAccessor & operator=(SkipAccessor &&) noexcept = delete;

    bool operator==(const SkipAccessor & other) {
        skip();

        return accessor == other.accessor;
    }

    bool operator!=(const SkipAccessor & other) {
        skip();

        return accessor != other.accessor;
    }

    value_type operator*() {
        skip();

        return *accessor;
    }

    SkipAccessor & operator++() {
        skip();

        ++accessor;

        return *this;
    }

private:
    Accessor accessor;
    Accessor end;

    std::size_t n;

    void skip() {
        for (; n > 0 && accessor != end; --n) {
            ++accessor;
        }
    }
};

#endif //STREAM_SKIPACCESSOR_H
