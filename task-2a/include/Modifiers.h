#ifndef STREAM_MODIFIERS_H
#define STREAM_MODIFIERS_H

#include "Utils.h"
#include "RangeAccessor.h"
#include "MapAccessor.h"
#include "FilterAccessor.h"
#include "SkipAccessor.h"
#include "GroupAccessor.h"

class get {
public:
    using StreamTag = FiniteStreamTag;

    get() = delete;
    explicit get(const std::size_t & _n)
            : n(_n) {
    }

    template <class Accessor>
    auto modify(Accessor begin, Accessor end) const {
        return std::pair{ RangeAccessor(begin, 0),
                          RangeAccessor(end, n) };
    }

private:
    const std::size_t n;
};

template <class Transform>
class map {
public:
    using StreamTag = InfiniteStreamTag;

    map() = delete;
    explicit map(const Transform & _transform)
            : transform(_transform) {
    }

    template <class Accessor>
    auto modify(Accessor begin, Accessor end) const {
        return std::pair{ MapAccessor(begin, transform),
                          MapAccessor(end, transform) };
    }

private:
    const Transform transform;
};

template <class Predicate>
class filter {
public:
    using StreamTag = InfiniteStreamTag;

    filter() = delete;
    explicit filter(const Predicate & _predicate)
            : predicate(_predicate) {
    }

    template <class Accessor>
    auto modify(Accessor begin, Accessor end) const {
        return std::pair{ FilterAccessor(begin, end, predicate),
                          FilterAccessor(end, end, predicate) };
    }

private:
    const Predicate predicate;
};

class skip {
public:
    using StreamTag = InfiniteStreamTag;

    skip() = delete;
    explicit skip(const std::size_t _n)
            : n(_n) {
    }

    template <class Accessor>
    auto modify(Accessor begin, Accessor end) const {
        return std::pair{ SkipAccessor(begin, end, n),
                          SkipAccessor(end, end, n) };
    }

        private:
    const std::size_t n;
};

class group {
public:
    using StreamTag = InfiniteStreamTag;

    group() = delete;
    explicit group(const std::size_t & _n)
            : n(_n) {
    }

    template <class Accessor>
    auto modify(Accessor begin, Accessor end) const {
        return std::pair{ GroupAccessor(begin, end, n),
                          GroupAccessor(end, end, n) };
    }

private:
    const std::size_t n;
};

#endif //STREAM_MODIFIERS_H
