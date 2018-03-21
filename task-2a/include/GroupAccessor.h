#ifndef STREAM_GROUPACCESSOR_H
#define STREAM_GROUPACCESSOR_H

#include <vector>

template <class Accessor>
class GroupAccessor {
public:
    using SuperValueType = typename std::iterator_traits<Accessor>::value_type;

    using difference_type = std::ptrdiff_t;
    using value_type = std::vector<SuperValueType>;
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::forward_iterator_tag;

    GroupAccessor() = delete;
    GroupAccessor(const Accessor & _accessor, const Accessor & _end,
                  std::size_t _n)
            : accessor(_accessor),
              end(_end),
              n(_n) {
    }

    GroupAccessor(const GroupAccessor &) = default;
    GroupAccessor(GroupAccessor &&) noexcept = default;

    ~GroupAccessor() = default;

    GroupAccessor & operator=(const GroupAccessor &) = delete;
    GroupAccessor & operator=(GroupAccessor &&) noexcept = delete;

    bool operator==(const GroupAccessor & other) {
        return accessor == other.accessor;
    }

    bool operator!=(const GroupAccessor & other) {
        return accessor != other.accessor;
    }

    value_type operator*() {
        auto res = std::vector<SuperValueType>();

        auto it = accessor;
        for (std::size_t i = 0; i < n && it != end; ++i, ++it) {
            res.push_back(*it);
        }

        return res;
    }

    GroupAccessor & operator++() {
        for (std::size_t i = 0; i < n && accessor != end; ++i) {
            ++accessor;
        }

        return *this;
    }

private:
    Accessor accessor;
    const Accessor end;

    const std::size_t n;
};

#endif //STREAM_GROUPACCESSOR_H
