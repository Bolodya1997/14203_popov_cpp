#ifndef STREAM_TERMINATORS_H
#define STREAM_TERMINATORS_H

#include <iostream>

#include "Utils.h"

template <class Accessor>
Accessor & firstOrThrow(Accessor & begin, Accessor & end,
                  const char * message = "") {
    while (begin != end) {
        if (begin.hasValue())
            return begin;
        ++begin;
    }

    throw std::out_of_range(message);
}

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

        auto res = identity(*firstOrThrow(begin, end, "empty stream"));
        for (++begin; begin != end; ++begin) {
            if (begin.hasValue())
                res = accum(res, *begin);
        }

        return res;
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

        auto res = *firstOrThrow(begin, end, "empty stream");
        for (++begin; begin != end; ++begin) {
            if (begin.hasValue())
                res = accum(res, *begin);
        }

        return res;
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
              delimiter(_delimiter) {
    }

    template <class Accessor, class StreamTag>
    std::ostream & terminate(Accessor begin, Accessor end, StreamTag) const {
        static_assert(std::is_same_v<StreamTag, FiniteStreamTag>);

        try {
            os << *firstOrThrow(begin, end, "");
        } catch (std::out_of_range) {
            return os;
        }

        for (++begin; begin != end; ++begin) {
            if (begin.hasValue())
                os << delimiter << *begin;
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

        auto res = std::vector<std::remove_reference_t<typename Accessor::Type>>();
        for (; begin != end; ++begin) {
            if (begin.hasValue()) {
                    res.push_back(std::move<typename Accessor::Type>(*begin));
            }
        }

        if constexpr (std::is_same_v<std::string &, typename Accessor::Type>) {
            for (auto && s : res) {
                std::cout << s << std::endl;
            }
        }

        return std::move(res);
    }
};

class nth {
public:
    explicit nth(size_t _n)
            : n(_n) {
    }

    template <class Accessor, class StreamTag>
    auto terminate(Accessor begin, Accessor end, StreamTag) const {
        for (std::size_t i = 0; i < n && begin != end; ++begin) {
            if (begin.hasValue())
                ++i;
        }

        while (begin != end) {
            if (begin.hasValue())
                return std::move(*begin);
            ++begin;
        }

        throw std::out_of_range("n is out of range");
    }

private:
    const std::size_t n;
};

#endif //STREAM_TERMINATORS_H
