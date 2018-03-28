#ifndef STREAM_TRACEABLE_H
#define STREAM_TRACEABLE_H

#include <cstddef>

class Traceable {
public:
    struct Trace {
        std::size_t copyConstructed = 0;
        std::size_t destructed = 0;
    };

    Traceable() = delete;

    explicit Traceable(Trace & _trace)
            : trace(&_trace) {
    }

    Traceable(const Traceable & other)
            : trace(other.trace) {
        ++trace->copyConstructed;
    }

    Traceable(Traceable && other) noexcept
            : trace(other.trace) {
        other.trace = nullptr;
    }

    ~Traceable() {
        if (trace != nullptr)
            ++trace->destructed;
    }

private:
    Trace * trace;
};

#endif //STREAM_TRACEABLE_H
