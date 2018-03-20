#ifndef STREAM_MODIFIERS_H
#define STREAM_MODIFIERS_H

#include "Stream.h"

class get {
public:
    using StreamTag = FiniteStreamTag;

    get() = delete;
    explicit get(const std::size_t & _n)
            : n(_n) {
    }

    template <class Accessor>
    auto rangeModify(Accessor begin, Accessor end) const {
        return std::pair{ RangeAccessor(begin, 0), RangeAccessor(end, n) };
    }

private:
    const std::size_t n;
};

template <class F>
class map {
public:
    using StreamTag = InfiniteStreamTag;

    map() = delete;
    explicit map(const F & _f)
            : f(_f) {
    }

    template <class In>
    auto mapModify(const In & value) const {
        return f(value);
    }

private:
    F f;
};

class filter {  //  TODO: filter
};

class skip {    //  TODO: skip
};

class group {
public:
    using StreamTag = InfiniteStreamTag;

    group() = delete;
    explicit group(const std::size_t & _n)
            : n(_n) {
    }

    template <class Accessor>
    auto groupModify(Accessor begin, Accessor end) const {
        return std::pair{ GroupAccessor(begin, end, n), GroupAccessor(end, end, n) };
    }

private:
    std::size_t n;
};

#endif //STREAM_MODIFIERS_H
