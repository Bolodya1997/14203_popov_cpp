#ifndef STREAM_GROUP_H
#define STREAM_GROUP_H

#include <vector>

#include "Utils.h"

class group {
public:
    using StreamTag = InfiniteStreamTag;

    template <class SAccessor>
    class Accessor {
    public:
        using SuperValueType = typename std::iterator_traits<SAccessor>::value_type;

        using difference_type = std::ptrdiff_t;
        using value_type = std::vector<SuperValueType>;
        using pointer = value_type *;
        using reference = value_type &;
        using iterator_category = std::forward_iterator_tag;

        Accessor() = delete;
        Accessor(const SAccessor & _sAccessor, const SAccessor & _end,
                 std::size_t _n)
                : sAccessor(_sAccessor),
                  end(_end),
                  n(_n) {
            if (n == 0)
                throw std::invalid_argument("n must be > 0");
        }

        Accessor(const Accessor &) = default;
        Accessor(Accessor &&) noexcept = default;

        ~Accessor() = default;

        Accessor & operator=(const Accessor &) = delete;
        Accessor & operator=(Accessor &&) noexcept = delete;

        bool operator==(const Accessor & other) {
            return sAccessor == other.sAccessor;
        }

        bool operator!=(const Accessor & other) {
            return sAccessor != other.sAccessor;
        }

        value_type operator*() {
            auto res = std::vector<SuperValueType>();

            auto it = sAccessor;
            for (std::size_t i = 0; i < n && it != end; ++i, ++it) {
                res.push_back(*it);
            }

            return res;
        }

        Accessor & operator++() {
            for (std::size_t i = 0; i < n && sAccessor != end; ++i) {
                ++sAccessor;
            }

            return *this;
        }

    private:
        SAccessor sAccessor;
        const SAccessor end;

        const std::size_t n;
    };

    group() = delete;
    explicit group(const std::size_t & _n)
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

#endif //STREAM_GROUP_H
