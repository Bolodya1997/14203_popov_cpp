#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Stream.h>
#include <filter.h>
#include <Terminators.h>

using testing::ElementsAreArray;

using ::filter;

TEST(filter, True) {
    auto res = Stream(1, 2)
            >> filter([](auto x) { return true; })
            >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 1, 2 }));
}

TEST(filter, False) {
    auto res = Stream(1, 2, 5, 10)
            >> filter([](auto x) { return false; })
            >> to_vector();

    ASSERT_TRUE(res.empty());
}

TEST(filter, Mod2) {
    auto res = Stream(1, 2, 3, 4, 5, 6)
            >> filter([](auto x) { return x % 2 == 0; })
            >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 2, 4, 6 }));
}

TEST(filter, Is3) {
    auto res = Stream(1, 2, 3, 4, 5, 6)
            >> filter([](auto x) { return x == 3; })
            >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 3 }));
}