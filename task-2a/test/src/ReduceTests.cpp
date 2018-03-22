#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Stream.h>
#include <Terminators.h>

using std::to_string;

//  NoIdentity

TEST(reduce, NoIdentity_Sum) {
    auto res = Stream(1, 2) >> reduce([](auto a, auto b) { return a + b; });

    ASSERT_EQ(res, 3);
}

TEST(reduce, NoIdentity_Empty) {
    ASSERT_THROW(Stream{} >> reduce([](auto a, auto) { return a; }), std::out_of_range);
}

//  Identity

TEST(reduce, Identity_Sum) {
    auto res = Stream(5, 15, 24) >> reduce([](auto x) { return to_string(x); },
                                           [](auto a, auto b) { return a + " " + to_string(b); });

    ASSERT_EQ(res, "5 15 24");
}

TEST(reduce, Identity_Empty) {
    ASSERT_THROW(Stream{} >> reduce([](auto x) { return x; },
                                    [](auto a, auto) { return a; }),
                 std::out_of_range);
}