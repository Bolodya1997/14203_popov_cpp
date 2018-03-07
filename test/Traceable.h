#ifndef LRU_TRACEABLE_H
#define LRU_TRACEABLE_H

#include <memory>
#include <utility>

using std::shared_ptr;

class Traceable {
public:
    struct Trace {
        int copyCalls = 0;
        int moveCalls = 0;
    };

    explicit Traceable(std::shared_ptr<Trace> trace) noexcept
            : trace(std::move(trace)) {
    }

    Traceable(const Traceable & other) noexcept {
        *this = other;
    }

    Traceable(Traceable && other) noexcept
            : trace(std::move(other.trace)) {
        trace->moveCalls++;
    }

    Traceable & operator=(const Traceable & other) noexcept {
        this->trace = other.trace;
        trace->copyCalls++;

        return *this;
    }

private:
    std::shared_ptr<Trace> trace;
};

#endif //LRU_TRACEABLE_H
