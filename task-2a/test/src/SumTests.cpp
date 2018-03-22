#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Stream.h>
#include <skip.h>
#include <Terminators.h>

using std::string;

TEST(sum, Int) {
    auto res = Stream(10, -15) >> sum();

    ASSERT_EQ(res, -5);
}

TEST(sum, String) {
    auto res = Stream(string("a"), string("bc"), string("d"), string("efg")) >> sum();

    ASSERT_EQ(res, "abcdefg");
}

TEST(sum, Empty) {
    ASSERT_THROW(Stream(1) >> skip(1) >> sum(), std::out_of_range);
}