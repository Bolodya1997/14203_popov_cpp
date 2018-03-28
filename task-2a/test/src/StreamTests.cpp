#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <list>
#include <memory>

#include <Stream.h>
#include <Terminators.h>
#include <get.h>

#include "Traceable.h"

using testing::ElementsAreArray;

using std::vector;
using std::string;
using std::list;
using std::unique_ptr;
using std::make_unique;
using std::move;

//  IteratorConstructor

TEST(Stream, IteratorConstructor_Pointer) {
    int a[] = { 0, 1, 3, 10, 19, 28 };
    int * start = a;

    auto res = Stream(start, start + 6) >> to_vector();

    ASSERT_THAT(a, ElementsAreArray({ 0, 1, 3, 10, 19, 28 }));
    ASSERT_THAT(res, ElementsAreArray({ 0, 1, 3, 10, 19, 28 }));
}

TEST(Stream, IteratorConstructor_Vector) {
    auto v = vector<string>{ "a", "b", "kek" };

    auto res = Stream(v.cbegin(), v.cend()) >> to_vector();

    ASSERT_THAT(v, ElementsAreArray({ "a", "b", "kek" }));
    ASSERT_THAT(res, ElementsAreArray({ "a", "b", "kek" }));
}

TEST(Stream, IteratorConstructor_Empty) {
    int * start = nullptr;

    auto res = Stream(start, start) >> to_vector();

    ASSERT_TRUE(res.empty());
}

TEST(Stream, IteratorConstructor_CopyTraceable) {
    Traceable::Trace traces[2];
    auto v = vector<Traceable>();
    v.emplace_back(traces[0]);
    v.emplace_back(traces[1]);

    Stream(v.cbegin(), v.cend()) >> to_vector();

    ASSERT_EQ(traces[0].copyConstructed, 1u);
    ASSERT_EQ(traces[0].destructed, 1u);

    ASSERT_EQ(traces[1].copyConstructed, 1u);
    ASSERT_EQ(traces[1].destructed, 1u);
}

TEST(Stream, IteratorConstructor_MoveTraceable) {
    Traceable::Trace traces[2];
    auto v = vector<Traceable>();
    v.emplace_back(traces[0]);
    v.emplace_back(traces[1]);

    Stream(v.begin(), v.end()) >> to_vector();

    ASSERT_EQ(traces[0].copyConstructed, 0u);
    ASSERT_EQ(traces[0].destructed, 1u);

    ASSERT_EQ(traces[1].copyConstructed, 0u);
    ASSERT_EQ(traces[1].destructed, 1u);
}

TEST(Stream, IteratorCounstructor_UniquePtr) {
    auto v = vector<unique_ptr<int>>();
    for (int i = 0; i < 10; ++i) {
        v.push_back(make_unique<int>(0));
    }

    Stream(v.begin(), v.end()) >> to_vector();
}

//  ContainerConstructorCopy

TEST(Stream, ContainerConstructorCopy_Vector) {
    auto v = vector{ 1, 2, 10, -1, 228 };

    auto res = Stream(v) >> to_vector();

    ASSERT_THAT(v, ElementsAreArray({ 1, 2, 10, -1, 228 }));
    ASSERT_THAT(res, ElementsAreArray({ 1, 2, 10, -1, 228 }));
}

TEST(Stream, ContainerConstructorCopy_List) {
    auto l = list<string>{ "a", "kek", "hello" };

    auto res = Stream(l) >> to_vector();

    ASSERT_THAT(l, ElementsAreArray({ "a", "kek", "hello" }));
    ASSERT_THAT(res, ElementsAreArray({ "a", "kek", "hello" }));
}

TEST(Stream, ContainerConstructorCopy_Traceable) {
    Traceable::Trace traces[2];
    auto v = vector<Traceable>();
    v.emplace_back(traces[0]);
    v.emplace_back(traces[1]);

    Stream(v) >> to_vector();

    ASSERT_EQ(traces[0].copyConstructed, 1u);
    ASSERT_EQ(traces[0].destructed, 1u);

    ASSERT_EQ(traces[1].copyConstructed, 1u);
    ASSERT_EQ(traces[1].destructed, 1u);
}

//  ContainerConstructorMove

TEST(Stream, ContainerConstructorMove_Vector) {
    auto v = vector{ 1, 2, 10, -1, 228 };

    auto res = Stream(move(v)) >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 1, 2, 10, -1, 228 }));
}

TEST(Stream, ContainerConstructorMove_List) {
    auto l = list<string>{ "a", "kek", "hello" };

    auto res = Stream(move(l)) >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ "a", "kek", "hello" }));
}

TEST(Stream, ContainerConstructorMove_Traceable) {
    Traceable::Trace traces[2];
    auto v = vector<Traceable>();
    v.emplace_back(traces[0]);
    v.emplace_back(traces[1]);

    Stream(move(v)) >> to_vector();

    ASSERT_EQ(traces[0].copyConstructed, 0u);
    ASSERT_EQ(traces[0].destructed, 1u);

    ASSERT_EQ(traces[1].copyConstructed, 0u);
    ASSERT_EQ(traces[1].destructed, 1u);
}

TEST(Stream, ContainerConstructorMove_UniquePtr) {
    auto v = vector<unique_ptr<int>>();
    for (int i = 0; i < 10; ++i) {
        v.push_back(make_unique<int>(0));
    }

    Stream(move(v)) >> to_vector();
}

//  InitializerListConstructor

TEST(Stream, InitializerListConstructor_Int) {
    auto res = Stream{ 1, 2, 5, 18, 227, 4899 } >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 1, 2, 5, 18, 227, 4899 }));
}

TEST(Stream, InitializerListConstructor_String) {
    auto res = Stream({ "The", "quick", "brown", "fox", "jumps", "..." }) >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ "The", "quick", "brown", "fox", "jumps", "..." }));
}

TEST(Stream, InitializerListConstructor_Traceable) {
    Traceable::Trace traces[2];

    Stream({ Traceable(traces[0]), Traceable(traces[1]) }) >> to_vector();

    ASSERT_EQ(traces[0].copyConstructed, 1u);
    ASSERT_EQ(traces[0].destructed, 2u);

    ASSERT_EQ(traces[1].copyConstructed, 1u);
    ASSERT_EQ(traces[1].destructed, 2u);
}

//  GeneratorConstructor

TEST(Stream, GeneratorConstructor_Int) {
    int a = 0;
    auto res = Stream([&a]() { return a++; })
            >> ::get(5)
            >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 0, 1, 2, 3, 4 }));
}

TEST(Stream, GeneratorConstructor_String) {
    auto res = Stream([]() { return "kek"; })
            >> ::get(2)
            >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ "kek", "kek" }));
}

//  VariadicConstructor

TEST(Stream, VariadicConstructor_1) {
    auto res = Stream(1) >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 1 }));
}

TEST(Stream, VariadicConstructor_2) {
    auto res = Stream(1, 2) >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 1, 2 }));
}

TEST(Stream, VariadicConstructor_Multiple) {
    auto res = Stream(1, 2, 5, 18, 227, 4899) >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ 1, 2, 5, 18, 227, 4899 }));
}

TEST(Stream, VariadicConstructor_String) {
    auto res = Stream({ string("The"), string("quick"), string("brown"),
                        string("fox"), string("jumps") }) >> to_vector();

    ASSERT_THAT(res, ElementsAreArray({ "The", "quick", "brown", "fox", "jumps" }));
}

TEST(Stream, VariadicConstructor_CopyTraceable) {
    Traceable::Trace traces[2];
    Traceable traceables[] = { Traceable(traces[0]), Traceable(traces[1]) };

    Stream(traceables[0], traceables[1]) >> to_vector();

    ASSERT_EQ(traces[0].copyConstructed, 1u);
    ASSERT_EQ(traces[0].destructed, 1u);

    ASSERT_EQ(traces[1].copyConstructed, 1u);
    ASSERT_EQ(traces[1].destructed, 1u);
}

TEST(Stream, VariadicConstructor_MoveTraceable) {
    Traceable::Trace traces[2];

    Stream(Traceable(traces[0]), Traceable(traces[1])) >> to_vector();

    ASSERT_EQ(traces[0].copyConstructed, 0u);
    ASSERT_EQ(traces[0].destructed, 1u);

    ASSERT_EQ(traces[1].copyConstructed, 0u);
    ASSERT_EQ(traces[1].destructed, 1u);
}

TEST(Stream, VariadicConstructor_UniquePtr) {
    Stream(make_unique<int>(0), make_unique<int>(0), make_unique<int>(0)) >> to_vector();
}