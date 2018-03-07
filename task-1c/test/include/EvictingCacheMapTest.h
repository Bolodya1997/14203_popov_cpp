#ifndef LRU_EVICTINGCACHEMAPTEST_H
#define LRU_EVICTINGCACHEMAPTEST_H

#include <list>

#include <gtest/gtest.h>

#include "Traceable.h"

class EvictingCacheMapTest: public ::testing::Test {
protected:
    EvictingCacheMapTest() = default;

    ~EvictingCacheMapTest() override = default;

    Traceable createTraceable();

private:
    std::list<Traceable::Trace> traces;
};

#endif //LRU_EVICTINGCACHEMAPTEST_H
