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

    int a = 0;
    Stream([&a]() { return a++; })
            >> filter([](auto x) { return x % 2 == 0; })
            >> ::get(5)
            >> filter([](auto x) { return x > 5; })
            >> skip(1)
            >> print_to(cout) << endl;
}
