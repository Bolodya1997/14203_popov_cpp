#include "Traceable.h"

//  Traceable::Trace

int
Traceable::Trace::
getCopyCalls() const noexcept {
    return copyCalls;
}

bool
Traceable::Trace::
isAlive() const noexcept {
    return alive;
}

//  Traceable

Traceable::
Traceable(Traceable::Trace & trace) noexcept
        : trace(trace) {
}

Traceable::
Traceable(const Traceable & other) noexcept
        : trace(other.trace) {
    ++trace.copyCalls;
}

Traceable::
~Traceable() {
    trace.alive = false;
}

const Traceable::Trace &
Traceable::
getTrace() const noexcept {
    return trace;
}
