#ifndef STREAM_NOIZY_H
#define STREAM_NOIZY_H

#include <iostream>

struct Noizy {
    Noizy() {
        std::cout << "constructor" << std::endl;
    }

    Noizy(const Noizy &) {
        std::cout << "copy constructor" << std::endl;
    }

    Noizy(Noizy && other) noexcept {
        other.alive = false;
    }

    ~Noizy() {
        if (alive)
            std::cout << "destructor" << std::endl;
    }

private:
    bool alive = true;
};

#endif //STREAM_NOIZY_H
