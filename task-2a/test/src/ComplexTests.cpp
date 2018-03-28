#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

#include <Stream.h>
#include <filter.h>
#include <get.h>
#include <group.h>
#include <map.h>
#include <skip.h>
#include <Terminators.h>

#include "Traceable.h"

using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::move;

using ::map;
using ::get;
using ::filter;

TEST(Complex, Traceable1) {
    Traceable::Trace traces[10];

    auto res = Stream(reinterpret_cast<Traceable::Trace *>(traces),
                      reinterpret_cast<Traceable::Trace *>(traces) + 10)
            >> map([](auto && trace) { return Traceable(trace); })
            >> skip(3)
            >> get(10)
            >> group(5)
            >> filter([](auto && v) { return v.size() < 5; })
            >> map([](auto && v) { return v.size(); })
            >> nth(0);

    ASSERT_EQ(res, 2u);

    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(traces[i].copyConstructed, 0u);
        if (i < 3)
            ASSERT_EQ(traces[i].destructed, 0u);
        else
            ASSERT_EQ(traces[i].destructed, 1u);
    }
}

TEST(Complex, Traceable2) {
    Traceable::Trace traces[10];
    auto v = vector<Traceable>();
    for (int i = 0; i < 10; ++i) {
        v.emplace_back(traces[i]);
    }

    auto res = Stream(v)
            >> skip(3)
            >> get(10)
            >> group(5)
            >> filter([](auto && v) { return v.size() < 5; })
            >> map([](auto && v) { return v.size(); })
            >> nth(0);

    ASSERT_EQ(res, 2u);

    for (int i = 0; i < 10; ++i) {
        if (i < 3) {
            ASSERT_EQ(traces[i].destructed, 0u);
            ASSERT_EQ(traces[i].copyConstructed, 0u);
        } else {
            ASSERT_EQ(traces[i].destructed, 1u);
            ASSERT_EQ(traces[i].copyConstructed, 1u);
        }
    }
}

TEST(Complex, UniquePtr1) {
    auto v = vector<unique_ptr<int>>();
    for (int i = 0; i < 20; ++i) {
        v.push_back(make_unique<int>(0));
    }

    int a = 0;
    auto res = Stream(std::move(v))
            >> map([&a](auto && ptr) {
                *ptr = a++;
                return move(ptr);
            })
            >> filter([](auto && ptr) {
                return *ptr > 5;
            })
            >> group(2)
            >> filter([](auto && v) {
                return v.size() == 2;
            })
            >> group(10)
            >> map([](auto && v) {
                return v.size();
            })
            >> nth(0);

    ASSERT_EQ(res, 7u);
}

TEST(Complex, UniquePtr2) {
    auto v = vector<unique_ptr<int>>();
    for (int i = 0; i < 20; ++i) {
        v.push_back(make_unique<int>(0));
    }

    int a = 0;
    auto res = Stream(v.begin(), v.end())
            >> map([&a](auto && ptr) {
                *ptr = a++;
                return move(ptr);
            })
            >> filter([](auto && ptr) {
                return *ptr > 5;
            })
            >> group(2)
            >> filter([](auto && v) {
                return v.size() == 2;
            })
            >> group(10)
            >> map([](auto && v) {
                return v.size();
            })
            >> nth(0);

    ASSERT_EQ(res, 7u);
}