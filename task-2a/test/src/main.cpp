#include <gtest/gtest.h>

#include <vector>

#include <Stream.h>
#include <map.h>
#include <get.h>
#include <filter.h>
#include <Terminators.h>

#include "Noizy.h"

using std::cout;
using std::endl;
using std::vector;

using ::map;
using ::get;

int main(int argc, char * * argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//
//    return RUN_ALL_TESTS();
//    Stream(1, 2);

    auto v = vector<Noizy>(1);
    auto s = Stream(v);

//    Stream{ 1, 2, 3, 4, 5 }
//            >> filter([](auto && x) { return true; })
//            >> map([](auto && x) -> auto && { return x; })
//            >> nth(0);
//            >> to_vector();
}
