#ifndef STREAM_ITERACCESSOR_H
#define STREAM_ITERACCESSOR_H

template <class T, class Iterator>
class IterAccessor {
public:
    using Type = T;

    IterAccessor() = delete;

    explicit IterAccessor(Iterator _iterator)
            : iterator(_iterator) {
    }

    IterAccessor(const IterAccessor &) = default;
    IterAccessor(IterAccessor &&) noexcept = default;

    ~IterAccessor() = default;

    IterAccessor & operator=(const IterAccessor &) = delete;
    IterAccessor & operator=(IterAccessor &&) = delete;

    bool operator==(const IterAccessor & other) const {
        return iterator == other.iterator;
    }

    bool operator!=(const IterAccessor & other) const {
        return iterator != other.iterator;
    }

    bool hasValue() const {
        return true;
    }

    Type operator*() const {
        if constexpr (std::is_const_v<Type>)
            return Type(*iterator);
        else
            return *iterator;
    }

    IterAccessor & operator++() {
        ++iterator;

        return *this;
    }

private:
    Iterator iterator;
};

#endif //STREAM_ITERACCESSOR_H
