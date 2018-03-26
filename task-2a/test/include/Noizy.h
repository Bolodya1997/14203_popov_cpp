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

    Noizy(Noizy &&) noexcept {
        std::cout << "move constructor" << std::endl;
    }

    ~Noizy() {
        std::cout << "destructor" << std::endl;
    }
};

#endif //STREAM_NOIZY_H
