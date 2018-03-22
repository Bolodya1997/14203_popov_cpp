#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Stream.h>
#include <Terminators.h>
#include <skip.h>

using testing::internal::CaptureStdout;
using testing::internal::GetCapturedStdout;

using std::ostream;
using std::cout;

//  NoDelim

TEST(print_to, NoDelim_Int) {
    CaptureStdout();

    ostream & _cout = Stream(2, 10) >> print_to(cout);

    ASSERT_EQ(&_cout, &cout);
    ASSERT_EQ(GetCapturedStdout(), "2 10");
}

TEST(print_to, NoDelim_CString) {
    CaptureStdout();

    ostream & _cout = Stream{ "one", "two", ":D" } >> print_to(cout);

    ASSERT_EQ(&_cout, &cout);
    ASSERT_EQ(GetCapturedStdout(), "one two :D");
}

TEST(print_to, NoDelim_Empty) {
    CaptureStdout();

    ostream & _cout = Stream(1)
            >> skip(1)
            >> print_to(cout);

    ASSERT_EQ(&_cout, &cout);
    ASSERT_EQ(GetCapturedStdout(), "");
}

//  NoDelim

TEST(print_to, Delim_Int) {
    CaptureStdout();

    ostream & _cout = Stream(2, 10) >> print_to(cout, "+");

    ASSERT_EQ(&_cout, &cout);
    ASSERT_EQ(GetCapturedStdout(), "2+10");
}

TEST(print_to, Delim_CString) {
    CaptureStdout();

    ostream & _cout = Stream{ "one", "two", ":D" } >> print_to(cout, "...");

    ASSERT_EQ(&_cout, &cout);
    ASSERT_EQ(GetCapturedStdout(), "one...two...:D");
}

TEST(print_to, Delim_Empty) {
    CaptureStdout();

    ostream & _cout = Stream(1)
            >> skip(1)
            >> print_to(cout, "oooooo");

    ASSERT_EQ(&_cout, &cout);
    ASSERT_EQ(GetCapturedStdout(), "");
}