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
        : trace(&trace) {
}

Traceable::
Traceable(const Traceable & other) noexcept {
    *this = other;
}

Traceable::Traceable(Traceable && other) noexcept {
    *this = std::move(other);
}

Traceable &
Traceable::
operator=(const Traceable & other) noexcept {
    if (this != &other) {
        trace = other.trace;
        ++trace->copyCalls;
    }

    return *this;
}

Traceable & Traceable::operator=(Traceable && other) noexcept {
    if (this != &other) {
        trace = other.trace;
        other.trace = nullptr;
    }

    return *this;
}

Traceable::
~Traceable() {
    if (trace != nullptr)
        trace->alive = false;
}

bool
Traceable::
operator==(const Traceable & other) const noexcept {
    return &trace == &other.trace;
}

bool
Traceable::
operator!=(const Traceable & other) const noexcept {
    return !(*this == other);
}

const Traceable::Trace &
Traceable::
getTrace() const noexcept {
    return *trace;
}
