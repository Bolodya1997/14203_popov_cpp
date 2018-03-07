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

        const int & getConstructorCalls() const noexcept;
        const int & getCopyCalls() const noexcept;
        const int & getDestructorCalls() const noexcept;

    private:
        int constructorCalls = 0;
        int copyCalls = 0;
        int destructorCalls = 0;

        friend class Traceable;
    };

    Traceable() = delete;
    explicit Traceable(Trace & trace) noexcept;

    Traceable(const Traceable & other) noexcept;
    Traceable(Traceable && other) noexcept;

    ~Traceable();

    const Trace & getTrace() const noexcept;

private:
    Trace & trace;
};

#endif //LRU_TRACEABLE_H
