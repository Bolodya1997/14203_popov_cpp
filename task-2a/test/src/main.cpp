#include <gtest/gtest.h>

#include <vector>
#include <string>

#include <Stream.h>
#include <TypeTraits.h>

using namespace std;

struct inc {
    using In = int;
    using Out = int;


    Out modify(In value) const {
        return value + 1;
    }
};

struct sum {
    using In = int;
    using Out = int;

    template <class SAccessor>
    Out terminate(SAccessor begin, SAccessor end) const {
        int sum = 0;
        while (begin != end) {
            sum += *begin;
            ++begin;
        }

        return sum;
    }
};

int foo() {
    return 1;
}

int main(int argc, char * * argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//
//    return RUN_ALL_TESTS();

    auto v = vector({1, 2, 3, 4, 5});

    auto s1 = Stream(v.begin(), v.end());
    auto s2 = Stream(v);
    auto s3 = Stream(1, 2, 5, 9);
    auto s4 = Stream{2, 2, 8};
    auto s5 = Stream(foo);

    auto _s1 = s1 | inc() | inc();
    _s1.print();

    auto _s2 = s2 | inc();
    _s2.print();

    auto _s3 = s3 | inc();
    _s3.print();

    auto _s4 = s4 | inc();
    _s4.print();

    auto _s5 = s5 | inc();
    _s5.print();

    cout << (_s4 | sum()) << endl;
}
