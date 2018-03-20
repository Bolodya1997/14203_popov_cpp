#ifndef STREAM_MODIFIERS_H
#define STREAM_MODIFIERS_H

#include "Stream.h"

class get {
public:
    using ResultTag = FiniteStreamTag;

    explicit get(const std::size_t & _n)
            : n(_n) {
    }

    template <class Accessor, class StreamTag>
    auto rangeModify(Accessor begin, Accessor end, StreamTag) const {
        auto newBegin = RangeAccessor(begin, 0);
        auto newEnd = newBegin;

        for (std::size_t i = 0; i < n && begin != end; ++i, ++begin) {
            ++newEnd;
        }

        if (begin == end)
            return std::pair{ newBegin, RangeAccessor(end, n) };
        return std::pair{ newBegin, newEnd };
    }

private:
    const std::size_t n;
};

template <class F>
class map {
public:
    using ResultTag = InfiniteStreamTag;

    explicit map(const F & _f)
            : f(_f) {
    }

    template <class In, class StreamTag>
    auto modify(const In & value, StreamTag) const {
        return f(value);
    }

private:
    F f;
};

class filter {  //  TODO: filter
};

class skip {    //  TODO: skip
};

class group {   //  TODO: group
};

#endif //STREAM_MODIFIERS_H
