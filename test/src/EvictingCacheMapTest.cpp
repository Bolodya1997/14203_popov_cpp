#include "EvictingCacheMapTest.h"

#include <EvictingCacheMap.h>

//  EvictingCacheMapTest

Traceable
EvictingCacheMapTest::
createTraceable() {
    return Traceable(traces.emplace_back());
}

//  tests

using std::pair;
using std::move;
using std::vector;

//  put

TEST_F(EvictingCacheMapTest, Put) {
    pair<Traceable &&, Traceable &&> kv = { createTraceable(), createTraceable() };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces
            = { kv.first.getTrace(), kv.second.getTrace() };

    auto map = EvictingCacheMap<Traceable, Traceable>(1);
    map.put(kv.first, kv.second);  //  +1 copy [k, v]

    ASSERT_EQ(kvTraces.first.getCopyCalls(), 1);
    ASSERT_EQ(kvTraces.second.getCopyCalls(), 1);
}

TEST_F(EvictingCacheMapTest, PutMoveK) {
    pair<Traceable &&, Traceable &&> kv = { createTraceable(), createTraceable() };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces
            = { kv.first.getTrace(), kv.second.getTrace() };

    auto map = EvictingCacheMap<Traceable, Traceable>(1);
    map.put(move(kv.first), kv.second);    //  +1 copy [v]

    ASSERT_EQ(kvTraces.first.getCopyCalls(), 0);
    ASSERT_EQ(kvTraces.second.getCopyCalls(), 1);
}

TEST_F(EvictingCacheMapTest, PutMoveV) {
    pair<Traceable &&, Traceable &&> kv = { createTraceable(), createTraceable() };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces
            = { kv.first.getTrace(), kv.second.getTrace() };

    auto map = EvictingCacheMap<Traceable, Traceable>(1);
    map.put(kv.first, move(kv.second));    //  +1 copy [k]

    ASSERT_EQ(kvTraces.first.getCopyCalls(), 1);
    ASSERT_EQ(kvTraces.second.getCopyCalls(), 0);
}

TEST_F(EvictingCacheMapTest, PutMoveKV) {
    pair<Traceable &&, Traceable &&> kv = { createTraceable(), createTraceable() };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces
            = { kv.first.getTrace(), kv.second.getTrace() };

    auto map = EvictingCacheMap<Traceable, Traceable>(1);
    map.put(move(kv.first), move(kv.second));

    ASSERT_EQ(kvTraces.first.getCopyCalls(), 0);
    ASSERT_EQ(kvTraces.second.getCopyCalls(), 0);
}

TEST_F(EvictingCacheMapTest, PutGet) {
    auto map = EvictingCacheMap<int, int>(2);
    map.put(1, 2);
    map.put(3, 4);

    map.get(3);     //  put {3, 4} on the top of the queue
    map.put(5, 6);  //  evict {1, 2}

    ASSERT_FALSE(map.get(1).has_value());
    ASSERT_EQ(map.get(3).value(), 4);
    ASSERT_EQ(map.get(5).value(), 6);
}

//  copy assignment

TEST_F(EvictingCacheMapTest, CopyAssignmentSelf) {
    pair<Traceable &&, Traceable &&> kv = { createTraceable(), createTraceable() };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces
            = { kv.first.getTrace(), kv.second.getTrace() };

    auto map = EvictingCacheMap<Traceable, Traceable>(1);
    map.put(move(kv.first), move(kv.second));

    map = map;

    ASSERT_EQ(kvTraces.first.getCopyCalls(), 0);
    ASSERT_EQ(kvTraces.second.getCopyCalls(), 0);
    ASSERT_TRUE(kvTraces.first.isAlive());
    ASSERT_TRUE(kvTraces.second.isAlive());
}

TEST_F(EvictingCacheMapTest, CopyAssignmentOther) {
    pair<Traceable &&, Traceable &&> kv[] = {
            { createTraceable(), createTraceable() },
            { createTraceable(), createTraceable() }
    };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces[] = {
            { kv[0].first.getTrace(), kv[0].second.getTrace() },
            { kv[1].first.getTrace(), kv[1].second.getTrace() }
    };

    auto map0 = EvictingCacheMap<Traceable, Traceable>(1);
    map0.put(move(kv[0].first), move(kv[0].second));

    auto map1 = EvictingCacheMap<Traceable, Traceable>(1);
    map1.put(move(kv[1].first), move(kv[1].second));

    map0 = map1;    // +1 destructor [0][k, v], +1 copy [1][k, v]

    ASSERT_EQ(kvTraces[0].first.getCopyCalls(), 0);
    ASSERT_EQ(kvTraces[0].second.getCopyCalls(), 0);
    ASSERT_FALSE(kvTraces[0].first.isAlive());
    ASSERT_FALSE(kvTraces[0].second.isAlive());

    ASSERT_EQ(kvTraces[1].first.getCopyCalls(), 1);
    ASSERT_EQ(kvTraces[1].second.getCopyCalls(), 1);
    ASSERT_TRUE(kvTraces[1].first.isAlive());
    ASSERT_TRUE(kvTraces[1].second.isAlive());
}

TEST_F(EvictingCacheMapTest, CopyAssignmentOrder) {
    auto map0 = EvictingCacheMap<int, int>(10);
    for (int i = 0; i < 10; ++i) {
        map0.put(i, i);
    }

    auto map1 = map0;
    ASSERT_EQ(map0.size(), map1.size());

    auto it0 = map0.begin();
    auto it1 = map1.begin();
    for (int i = 0; i < 10; ++i, ++it0, ++it1) {
        ASSERT_EQ(it0->first, it1->first);
        ASSERT_EQ(it0->second, it1->second);
    }
}

//  move assignment

TEST_F(EvictingCacheMapTest, MoveAssignmentSelf) {
    pair<Traceable &&, Traceable &&> kv = { createTraceable(), createTraceable() };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces
            = { kv.first.getTrace(), kv.second.getTrace() };

    auto map = EvictingCacheMap<Traceable, Traceable>(1);
    map.put(move(kv.first), move(kv.second));

    map = map;

    ASSERT_EQ(kvTraces.first.getCopyCalls(), 0);
    ASSERT_EQ(kvTraces.second.getCopyCalls(), 0);
    ASSERT_TRUE(kvTraces.first.isAlive());
    ASSERT_TRUE(kvTraces.second.isAlive());
}

TEST_F(EvictingCacheMapTest, MoveAssignmentOther) {
    pair<Traceable &&, Traceable &&> kv[] = {
            { createTraceable(), createTraceable() },
            { createTraceable(), createTraceable() }
    };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces[] = {
            { kv[0].first.getTrace(), kv[0].second.getTrace() },
            { kv[1].first.getTrace(), kv[1].second.getTrace() }
    };

    auto map0 = EvictingCacheMap<Traceable, Traceable>(1);
    map0.put(move(kv[0].first), move(kv[0].second));

    auto map1 = EvictingCacheMap<Traceable, Traceable>(1);
    map1.put(move(kv[1].first), move(kv[1].second));

    map0 = map1;    // +1 destructor [0][k, v]

    ASSERT_EQ(kvTraces[0].first.getCopyCalls(), 0);
    ASSERT_EQ(kvTraces[0].second.getCopyCalls(), 0);
    ASSERT_FALSE(kvTraces[0].first.isAlive());
    ASSERT_FALSE(kvTraces[0].second.isAlive());

    ASSERT_EQ(kvTraces[1].first.getCopyCalls(), 0);
    ASSERT_EQ(kvTraces[1].second.getCopyCalls(), 0);
    ASSERT_TRUE(kvTraces[1].first.isAlive());
    ASSERT_TRUE(kvTraces[1].second.isAlive());
}

TEST_F(EvictingCacheMapTest, MoveAssignmentOrder) {
    auto map0 = EvictingCacheMap<int, int>(10);
    for (int i = 0; i < 10; ++i) {
        map0.put(i, i);
    }

    auto vec = vector<pair<const int, int>>();
    for (auto && kv : map0) {
        vec.push_back(kv);
    }

    auto map1 = map0;
    ASSERT_EQ(map1.size(), 10);

    auto it = map1.begin();
    for (int i = 0; i < 10; ++i, ++it) {
        ASSERT_EQ(it->first, vec[i].first);
        ASSERT_EQ(it->second, vec[i].second);
    }
}