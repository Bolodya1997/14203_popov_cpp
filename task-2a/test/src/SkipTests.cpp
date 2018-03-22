#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Stream.h>
#include <skip.h>
#include <Terminators.h>

using testing::ElementsAreArray;

TEST(skip, Nothing) {
    auto res = Stream(10, 217)
            >> skip(0)
            >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 10, 217 }));
}

TEST(skip, All) {
    auto res = Stream(10, 217, 555, 278)
            >> skip(100)
            >> to_vector();

    ASSERT_TRUE(res.empty());
}

TEST(skip, Two) {
    auto res = Stream(1, 10, 22, 5, 278)
            >> skip(2)
            >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 22, 5, 278 }));
}