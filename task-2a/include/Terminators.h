#ifndef STREAM_TERMINATORS_H
#define STREAM_TERMINATORS_H

#include <iostream>

#include "Utils.h"

template <class IdentityFn, class Accumulator>
class reduce {
public:
    reduce(const IdentityFn & _identity, const Accumulator & _accum)
            : identity(_identity),
              accum(_accum) {
    }

    template <class Accessor, class StreamTag>
    auto terminate(Accessor begin, Accessor end, StreamTag) const {
        static_assert(std::is_same_v<StreamTag, FiniteStreamTag>);

        if (begin == end)
            throw std::out_of_range("empty stream");

        decltype(std::declval<IdentityFn>()(*begin)) result = identity(*begin);
        while (++begin != end) {
            result = accum(result, *begin);
        }
    }

private:
    const IdentityFn & identity;
    const Accumulator & accum;
};

template <class Accumulator>
class reduce<int, Accumulator> {
public:
    explicit reduce(const Accumulator & _accum)
            : accum(_accum) {
    }

    template <class Accessor, class StreamTag>
    auto terminate(Accessor begin, Accessor end, StreamTag) const {
        static_assert(std::is_same_v<StreamTag, FiniteStreamTag>);

        if (begin == end)
            throw std::out_of_range("empty stream");

        decltype(*begin) result = *begin;
        while (++begin != end) {
            result = accum(result, *begin);
        }
    }

private:
    const Accumulator & accum;
};

template <class Accumulator>
explicit reduce(Accumulator) -> reduce<int, Accumulator>;

class sum {
public:
    sum() = default;

    template <class Accessor, class StreamTag>
    auto terminate(Accessor begin, Accessor end, StreamTag tag) const {
        return reduce([](auto a, auto b) { return a + b; })
                .terminate(begin, end, tag);
    }
};

class print_to {
public:
    explicit print_to(std::ostream & _os, const char * _delimiter = " ")
            : os(_os),
              delimiter(_delimiter){
    }

    template <class Accessor, class StreamTag>
    std::ostream & terminate(Accessor begin, Accessor end, StreamTag) const {
        static_assert(std::is_same_v<StreamTag, FiniteStreamTag>);

        for (auto ac = begin; ac != end; ++ac) {
            os << *ac << delimiter;
        }

        return os;
    }

private:
    std::ostream & os;
    const char *const delimiter;
};

class to_vector {
public:
    to_vector() = default;

    template <class Accessor, class StreamTag>
    auto terminate(Accessor begin, Accessor end, StreamTag) const {
        static_assert(std::is_same_v<StreamTag, FiniteStreamTag>);

        return std::vector<typename std::iterator_traits<Accessor>::value_type>(begin, end);
    }
};

class nth {
public:
    nth(size_t _n)
            : n(_n) {
    }

    template <class Accessor, class StreamTag>
    auto terminate(Accessor begin, Accessor end, StreamTag) const {
        for (std::size_t i = 0; i < n; ++i) {
            if (++begin == end)
                throw std::out_of_range("n is out of range");
        }

        return begin;
    }

private:
    const std::size_t n;
};

#endif //STREAM_TERMINATORS_H
