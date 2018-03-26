#include <gtest/gtest.h>

#include <Stream.h>
#include <map.h>
#include <Terminators.h>

using std::cout;
using std::endl;

using ::map;

int main(int argc, char * * argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//
//    return RUN_ALL_TESTS();
//    Stream(1, 2);

    int a = 0;
    auto b = Stream([&a]() -> int & { return a; })
            >> map([](int & x) -> int & { return ++x; })
            >> nth(5);
    cout << b << endl;
}
