#ifndef LRU_TRACEABLE_H
#define LRU_TRACEABLE_H

#include <memory>
#include <utility>
#include <iostream>

class Traceable final {
public:
    class Trace final {
    public:
        Trace() = default;

        const int & getCopyCalls() const noexcept;
        const bool & isAlive() const noexcept;

    private:
        int copyCalls = 0;
        bool alive = true;

        friend class Traceable;
    };

    Traceable() = delete;
    explicit Traceable(Trace & trace) noexcept;

    Traceable(const Traceable & other) noexcept;
    Traceable(Traceable && other) noexcept = default;

    ~Traceable();

    const Trace & getTrace() const noexcept;

private:
    Trace & trace;
};

#endif //LRU_TRACEABLE_H
