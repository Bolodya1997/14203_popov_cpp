#ifndef LRU_TRACEABLE_H
#define LRU_TRACEABLE_H

#include <memory>
#include <utility>
#include <iostream>
#include <functional>

class Traceable final {
public:
    class Trace final {
    public:
        Trace() = default;

        int getCopyCalls() const noexcept;
        bool isAlive() const noexcept;

    private:
        int copyCalls = 0;
        bool alive = true;

        friend class Traceable;
    };

    Traceable() = delete;
    explicit Traceable(Trace & trace) noexcept;

    Traceable(const Traceable & other) noexcept;
    Traceable(Traceable && other) noexcept;

    Traceable & operator=(const Traceable & other) noexcept;
    Traceable & operator=(Traceable && other) noexcept;

    ~Traceable();

    bool operator==(const Traceable & other) const noexcept;
    bool operator!=(const Traceable & other) const noexcept;

    const Trace & getTrace() const noexcept;

private:
    Trace * trace;
};

template<>
struct std::hash<Traceable> {

    std::size_t operator()(const Traceable & traceable) const {
        return reinterpret_cast<std::size_t>(&traceable.getTrace());
    }
};

#endif //LRU_TRACEABLE_H
