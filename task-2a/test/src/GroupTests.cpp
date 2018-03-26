#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Stream.h>
#include <group.h>
#include <skip.h>
#include <Terminators.h>

using testing::ElementsAreArray;

TEST(group, Zero) {
    ASSERT_THROW(Stream(1, 2) >> group(0), std::invalid_argument);
}

TEST(group, Empty) {
    auto res = Stream(1)
            >> skip(1)
            >> group(10)
            >> to_vector();

    ASSERT_TRUE(res[0].empty());
}

TEST(group, Two) {
    auto res = Stream(1, 2, 5, 9, 100, 19, 17)
            >> group(2)
            >> to_vector();

    ASSERT_THAT(res[0], ElementsAreArray({ 1, 2 }));
    ASSERT_THAT(res[1], ElementsAreArray({ 5, 9 }));
    ASSERT_THAT(res[2], ElementsAreArray({ 100, 19 }));
    ASSERT_THAT(res[3], ElementsAreArray({ 17 }));
}

TEST(group, All) {
    auto res = Stream(1, 2, 3, 100, 34)
            >> group(100)
            >> to_vector();

    ASSERT_THAT(res[0], ElementsAreArray({ 1, 2, 3, 100, 34 }));
}