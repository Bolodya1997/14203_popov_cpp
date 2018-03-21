#ifndef STREAM_ACCESSOR_H
#define STREAM_ACCESSOR_H

#include <iterator>

template <class Accessor, class Transform>
class MapAccessor {
public:
    using SuperValueType = typename std::iterator_traits<Accessor>::value_type;

    using difference_type = std::ptrdiff_t;
    using value_type = decltype(std::declval<Transform>()(std::declval<SuperValueType>()));
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::forward_iterator_tag;

    MapAccessor() = delete;
    MapAccessor(const Accessor & _accessor, const Transform & _transform)
            : accessor(_accessor),
              transform(_transform) {
    }

    MapAccessor(const MapAccessor &) = default;
    MapAccessor(MapAccessor &&) noexcept = default;

    ~MapAccessor() = default;

    MapAccessor & operator=(const MapAccessor &) = delete;
    MapAccessor & operator=(MapAccessor &&) noexcept = delete;

    bool operator==(const MapAccessor & other) {
        return accessor == other.accessor;
    }

    bool operator!=(const MapAccessor & other) {
        return accessor != other.accessor;
    }

    value_type operator*() {
        return transform(*accessor);
    }

    MapAccessor & operator++() {
        ++accessor;

        return *this;
    }

private:
    Accessor accessor;

    const Transform & transform;
};

#endif //STREAM_ACCESSOR_H
