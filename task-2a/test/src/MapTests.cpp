#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

#include <Stream.h>
#include <map.h>
#include <Terminators.h>

using testing::ElementsAreArray;

using std::string;
using std::to_string;

using ::map;

TEST(map, IntInt) {
    auto res = Stream(1, 4)
            >> map([](auto x) { return x + x; })
            >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 2, 8 }));
}

TEST(map, IntString) {
    auto res = Stream(3, 10, 228, 115)
            >> map([](auto x) { return to_string(x); })
            >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ "3", "10", "228", "115" }));
}

TEST(map, StringInt) {
    auto res = Stream({ string("a"), string("ab"), string("abccc") })
            >> map([](auto s) { return s.size(); })
            >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 1, 2, 5 }));
}