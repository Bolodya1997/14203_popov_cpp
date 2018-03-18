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

    int array[] = {1, 2, 3, 4, 5};
    int * start = array;

    auto _int = Stream(start, start + 5);
    auto _string = Stream([]() { return string("kek"); });

    _int >> print_to(cout);

    auto a = Stream(1, 2, 3) >> sum();
    cout << a;
}
