#ifndef STREAM_TERMINATORS_H
#define STREAM_TERMINATORS_H

#include <iostream>

class reduce {  //  TODO: reduce
};

class sum {
public:
    template <class SAccessor>
    auto terminate(SAccessor begin, SAccessor end) const
    -> typename std::iterator_traits<SAccessor>::value_type {
        auto res = *begin;
        while (++begin != end) {
            res += *begin;
        }

        return res;
    }
};

class print_to {
public:
    explicit print_to(std::ostream & _os, const char * _delimiter = " ")
            : os(_os),
              delimiter(_delimiter){
    }

    template <class SAccessor>
    std::ostream & terminate(SAccessor begin, SAccessor end) const {
        for (auto ac = begin; ac != end; ++ac) {
            os << *ac << delimiter;
        }

        return os;
    }

private:
    std::ostream & os;
    const char * delimiter;
};

class to_vector {   //  TODO: to_vector
};

class nth {         //  TODO: nth
};

#endif //STREAM_TERMINATORS_H
