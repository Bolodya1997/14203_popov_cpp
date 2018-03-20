#include <gtest/gtest.h>

#include <vector>
#include <string>

#include <Stream.h>
#include <Modifiers.h>
#include <Terminators.h>

using namespace std;

int foo() {
    return 1;
}

int main(int argc, char * * argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//
//    return RUN_ALL_TESTS();

    auto vi = vector{ 1, 2, 3, 4, 5 };
    auto vs = vector<string>{ "a", "b", "c" };

    auto _int = Stream(vi.begin(), vi.end());
    auto _string = Stream([]() { return string("kek"); });

    Stream([]() { return 1; })
            >> group(10)
            >> group(1)
            >> ::get(3)
            >> ::map([](auto v) { return v[0]; })
            >> ::map([](auto v) { return v.size(); })
            >> print_to(cout) << endl;
}
