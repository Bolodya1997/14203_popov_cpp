#include "EvictingCacheMapTest.h"

#include <EvictingCacheMap.h>

//  EvictingCacheMapTest

Traceable
EvictingCacheMapTest::
createTraceable() {
    return Traceable(traces.emplace_back());
}

#define ASSERT_TRACE_EQ(trace, constructorCalls, copyCalls, destructorCalls) \
do {    \
    ASSERT_EQ(trace.getConstructorCalls(), constructorCalls); \
    ASSERT_EQ(trace.getCopyCalls(), copyCalls); \
    ASSERT_EQ(trace.getDestructorCalls(), destructorCalls); \
} while (false)

//  constructor calls

TEST_F(EvictingCacheMapTest, CopyConstructor) {
    auto && key = createTraceable();     //  +1 constructor
    auto & keyTrace = key.getTrace();

    auto && value = createTraceable();   //  +1 constructor
    auto & valueTrace = value.getTrace();

    auto map1 = EvictingCacheMap<Traceable, Traceable>(1);
    map1.put(key, value);

    auto map2 = map1;                   //  +1 copy (key, value)

    ASSERT_TRACE_EQ(keyTrace, 1, 1, 0);
    ASSERT_TRACE_EQ(valueTrace, 1, 1, 0);
}
