#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Stream.h>
#include <Terminators.h>

#include <vector>
#include <list>

//  IteratorConstructor

TEST(Stream, IteratorConstructor_Pointer) {
    int a[] = { 0, 1, 3, 10, 19, 28 };
    int *start = a;

    auto res = Stream(start, start + 6) >> to_vector();
    ASSERT_THAT(res, ::testing::ElementsAreArray(a));
}

TEST(Stream, IteratorConstructor_Vector) {
    auto v = std::vector<std::string>{ "a", "b", "kek" };

    auto res = Stream(v.begin(), v.end()) >> to_vector();
    ASSERT_THAT(res, ::testing::ElementsAreArray(v.begin(), v.end()));
}

TEST(Stream, IteratorConstructor_Empty) {
    int *start = nullptr;

    auto res = Stream(start, start) >> to_vector();
    ASSERT_THAT(res, ::testing::ElementsAreArray(start, start));
}

//  ContainerConstructorCopy

TEST(Stream, ContainerConstructorCopy_Vector) {
    auto v = std::vector{ 1, 2, 10, -1, 228 };

    auto res = Stream(v) >> to_vector();
    ASSERT_THAT(res, ::testing::ElementsAreArray(v.begin(), v.end()));
}

TEST(Stream, ContainerConstructorCopy_List) {
    auto l = std::list<std::string>{ "a", "kek", "hello" };

    auto res = Stream(l) >> to_vector();
    ASSERT_THAT(res, ::testing::ElementsAreArray(l.begin(), l.end()));
}

//  ContainerConstructorMove

TEST(Stream, ContainerConstructorMove_Vector) {
    auto v = std::vector{ 1, 2, 10, -1, 228 };

    auto res = Stream(std::move(v)) >> to_vector();
    ASSERT_THAT(res, ::testing::ElementsAreArray({ 1, 2, 10, -1, 228 }));
}

TEST(Stream, ContainerConstructorMove_List) {
    auto l = std::list<std::string>{ "a", "kek", "hello" };

    auto res = Stream(std::move(l)) >> to_vector();
    ASSERT_THAT(res, ::testing::ElementsAreArray({ "a", "kek", "hello" }));
}