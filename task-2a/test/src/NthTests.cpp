#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Stream.h>
#include <Terminators.h>

TEST(nth, Zero) {
    auto res = Stream(2, 10) >> nth(0);

    ASSERT_EQ(res, 2);
}

TEST(nth, OutOfRange) {
    ASSERT_THROW(Stream(2, 10) >> nth(10), std::out_of_range);
}