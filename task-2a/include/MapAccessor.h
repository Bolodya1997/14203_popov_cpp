#ifndef STREAM_ACCESSOR_H
#define STREAM_ACCESSOR_H

#include <iterator>

template <class Accessor, class Modifier>
class MapAccessor {
public:
    using SuperValueType = typename std::iterator_traits<Accessor>::value_type;

    using difference_type = std::ptrdiff_t;
    using value_type = decltype(std::declval<Modifier>().mapModify(std::declval<SuperValueType>()));
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::forward_iterator_tag;

    MapAccessor() = delete;
    explicit MapAccessor(const Accessor & _accessor, const Modifier & _modifier)
            : accessor(_accessor),
              modifier(_modifier) {
    }

    MapAccessor(const MapAccessor &) = default;
    MapAccessor(MapAccessor &&) noexcept = default;

    ~MapAccessor() = default;

    MapAccessor & operator=(const MapAccessor &) = default;
    MapAccessor & operator=(MapAccessor &&) noexcept = default;

    bool operator==(const MapAccessor & other) {
        return accessor == other.accessor;
    }

    bool operator!=(const MapAccessor & other) {
        return accessor != other.accessor;
    }

    value_type operator*() const {
        return modifier.mapModify(*accessor);
    }

    MapAccessor & operator++() {
        ++accessor;

        return *this;
    }

private:
    Accessor accessor;

    Modifier modifier;
};

#endif //STREAM_ACCESSOR_H
