#ifndef STREAM_MAP_H
#define STREAM_MAP_H

#include <iterator>

#include "Utils.h"

template <class Transform>
class map {
public:
    using StreamTag = InfiniteStreamTag;

    template <class SAccessor>
    class Accessor {
    public:
        using Type = decltype(std::declval<Transform>()(std::declval<typename SAccessor::Type>()));

        Accessor() = delete;

        Accessor(const SAccessor & _sAccessor, Transform & _transform)
                : sAccessor(_sAccessor),
                  transform(_transform) {
        }

        Accessor(const Accessor &) = default;

        ~Accessor() = default;

        Accessor & operator=(const Accessor &) = delete;

        bool operator==(const Accessor & other) const {
            return sAccessor == other.sAccessor;
        }

        bool operator!=(const Accessor & other) const {
            return sAccessor != other.sAccessor;
        }

        bool hasValue() {
            return sAccessor.hasValue();
        }

        Type operator*() {
            return transform(*sAccessor);
        }

        Accessor & operator++() {
            ++sAccessor;

            return *this;
        }

    private:
        SAccessor sAccessor;

        Transform & transform;
    };

    map() = delete;

    explicit map(Transform & _transform)
            : transform(_transform) {
    }

    explicit map(Transform && _transform)
            : transform(std::move(_transform)) {
    }

    map(const map &) = default;
    map(map &&) noexcept = default;

    template <class SAccessor>
    auto modify(const SAccessor & begin, const SAccessor & end) {
        return std::pair{ Accessor<SAccessor>(begin, transform),
                          Accessor<SAccessor>(end, transform) };
    }

private:
    Transform transform;
};

#endif //STREAM_MAP_H
