#include <gtest/gtest.h>

#include <vector>

#include <Stream.h>
#include <map.h>
#include <get.h>
#include <filter.h>
#include <skip.h>
#include <group.h>
#include <Terminators.h>

#include "Noizy.h"

using std::cout;
using std::endl;
using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::to_string;

using ::map;
using ::get;

int main(int argc, char * * argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();

//    auto v = vector<unique_ptr<int>>();
//    for (int i = 0; i < 10; ++i) {
//        v.push_back(make_unique<int>(0));
//    }

//    for (auto && ptr : v) {
//        cout << *ptr << endl;
//    }

//    int a = 0;
//    Stream(std::move(v))
//            >> map([&a](auto && ptr) -> auto && {
//                *ptr += ++a;
//                return ptr;
//            })
//            >> filter([](auto && ptr) {
//                return *ptr % 2 == 0;
//            })
//            >> skip(2)
//            >> group(3)
//            >> map([](auto && v) -> auto {
//                return std::move(v[0]);
//            })
//            >> map([](auto && ptr) -> auto {
//                return to_string(*ptr);
//            })
//            >> print_to(cout, "...") << endl;
}
