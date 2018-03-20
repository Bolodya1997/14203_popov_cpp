#ifndef STREAM_ACCESSOR_H
#define STREAM_ACCESSOR_H

#include <iterator>

template <class Accessor, class Modifier, class StreamTag>
class ModifierAccessor {
public:
    using SuperValueType = typename std::iterator_traits<Accessor>::value_type;

    using difference_type = std::ptrdiff_t;
    using value_type = decltype(std::declval<Modifier>().modify(std::declval<SuperValueType>(),
                                                                std::declval<StreamTag>()));
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::forward_iterator_tag;

    ModifierAccessor() = delete;
    explicit ModifierAccessor(const Accessor & _accessor, const Modifier & _modifier)
            : accessor(_accessor),
              modifier(_modifier) {
    }

    ModifierAccessor(const ModifierAccessor & other) = default;
    ModifierAccessor(ModifierAccessor && other) noexcept = default;

    ~ModifierAccessor() = default;

    ModifierAccessor & operator=(const ModifierAccessor & other) = default;
    ModifierAccessor & operator=(ModifierAccessor && oher) noexcept = default;

    bool operator==(const ModifierAccessor & other) {
        return accessor == other.accessor;
    }

    bool operator!=(const ModifierAccessor & other) {
        return accessor != other.accessor;
    }

    value_type operator*() const {
        return modifier.modify(*accessor, StreamTag());
    }

    ModifierAccessor & operator++() {
        ++accessor;

        return *this;
    }

private:
    Accessor accessor;

    Modifier modifier;
};

#endif //STREAM_ACCESSOR_H
