#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Stream.h>
#include <get.h>
#include <Terminators.h>

using testing::ElementsAreArray;

using ::get;

//  Finite

TEST(get, Finite0) {
    auto res = Stream(1, 2)
            >> get(0)
            >> to_vector();

    ASSERT_TRUE(res.empty());
}

TEST(get, FiniteN) {
    auto res = Stream(1, 2, 5, 6, 1900, 234)
            >> get(4)
            >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 1, 2, 5, 6 }));
}

TEST(get, FiniteOverflow) {
    auto res = Stream(1, 2, 5, 6, 1900, 234)
            >> get(100)
            >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 1, 2, 5, 6, 1900, 234 }));
}

//  Infinite

TEST(get, Infinite0) {
    int a = 0;
    auto res = Stream([&a]() { return a++; })
            >> get(0)
            >> to_vector();

    ASSERT_TRUE(res.empty());
}

TEST(get, InfiniteN) {
    int a = 0;
    auto res = Stream([&a]() { return a++; })
            >> get(4)
            >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 0, 1, 2, 3 }));
}