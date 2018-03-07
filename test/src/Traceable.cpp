#include "Traceable.h"

//  Traceable::Trace

const int &
Traceable::Trace::
getConstructorCalls() const noexcept {
    return constructorCalls;
}

const int &
Traceable::Trace::
getCopyCalls() const noexcept {
    return copyCalls;
}

const int &
Traceable::Trace::
getDestructorCalls() const noexcept {
    return destructorCalls;
}

//  Traceable

Traceable::
Traceable(Traceable::Trace & trace) noexcept
        : trace(trace) {
    ++trace.constructorCalls;
    std::cout << "Traceable()" << std::endl;
}

Traceable::
Traceable(const Traceable & other) noexcept
        : trace(other.trace) {
    ++trace.copyCalls;
    std::cout << "Traceable(): copy" << std::endl;
}

Traceable::
Traceable(Traceable && other) noexcept
        : trace(other.trace) {
}

Traceable::
~Traceable() {
    ++trace.destructorCalls;
    std::cout << "~Traceable()" << std::endl;
}

const Traceable::Trace &
Traceable::
getTrace() const noexcept {
    return trace;
}
