#include "EvictingCacheMapTest.h"

#include <gmock/gmock.h>

#include <EvictingCacheMap.h>

using std::pair;
using std::move;
using std::vector;
using std::list;
using std::size_t;

//  put

TEST_F(EvictingCacheMapTest, Put) {
    pair<Traceable, Traceable> kv = { createTraceable(), createTraceable() };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces
            = { kv.first.getTrace(), kv.second.getTrace() };

    auto map = EvictingCacheMap<Traceable, Traceable>(1);
    map.put(kv.first, kv.second);  //  +1 copy [k, v]

    ASSERT_LE(kvTraces.first.getCopyCalls(), 1);
    ASSERT_LE(kvTraces.second.getCopyCalls(), 1);
}

TEST_F(EvictingCacheMapTest, PutMoveK) {
    pair<Traceable, Traceable> kv = { createTraceable(), createTraceable() };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces
            = { kv.first.getTrace(), kv.second.getTrace() };

    auto map = EvictingCacheMap<Traceable, Traceable>(1);
    map.put(move(kv.first), kv.second);    //  +1 copy [v]

    ASSERT_LE(kvTraces.first.getCopyCalls(), 0);
    ASSERT_LE(kvTraces.second.getCopyCalls(), 1);
}

TEST_F(EvictingCacheMapTest, PutMoveV) {
    pair<Traceable, Traceable> kv = { createTraceable(), createTraceable() };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces
            = { kv.first.getTrace(), kv.second.getTrace() };

    auto map = EvictingCacheMap<Traceable, Traceable>(1);
    map.put(kv.first, move(kv.second));    //  +1 copy [k]

    ASSERT_LE(kvTraces.first.getCopyCalls(), 1);
    ASSERT_LE(kvTraces.second.getCopyCalls(), 0);
}

TEST_F(EvictingCacheMapTest, PutMoveKV) {
    pair<Traceable, Traceable> kv = { createTraceable(), createTraceable() };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces
            = { kv.first.getTrace(), kv.second.getTrace() };

    auto map = EvictingCacheMap<Traceable, Traceable>(1);
    map.put(move(kv.first), move(kv.second));

    ASSERT_LE(kvTraces.first.getCopyCalls(), 0);
    ASSERT_LE(kvTraces.second.getCopyCalls(), 0);
}

TEST_F(EvictingCacheMapTest, PutGet) {
    auto map = EvictingCacheMap<int, int>(5);
    map.put(1, 2);
    map.put(3, 4);
    map.put(5, 6);
    map.put(7, 8);
    map.put(9, 0);

    map.get(3);     //  put {3, 4} on the top of the queue
    map.put(0, 0);  //  evict {1, 2}

    ASSERT_FALSE(map.get(1).has_value());
    ASSERT_EQ(map.get(3).value(), 4);
    ASSERT_EQ(map.get(5).value(), 6);
    ASSERT_EQ(map.get(7).value(), 8);
    ASSERT_EQ(map.get(9).value(), 0);
    ASSERT_EQ(map.get(0).value(), 0);
}

TEST_F(EvictingCacheMapTest, PutReplace) {
    auto map = EvictingCacheMap<int, int>(4);
    map.put(1, 1);
    map.put(2, 2);
    map.put(3, 3);
    map.put(1, 2);

    ASSERT_EQ(map.size(), 3u);
    ASSERT_EQ(map.get(1).value(), 2);
    ASSERT_EQ(map.get(2).value(), 2);
    ASSERT_EQ(map.get(3).value(), 3);
}

//  copy assignment

TEST_F(EvictingCacheMapTest, CopyAssignmentSelf) {
    pair<Traceable, Traceable> kv = { createTraceable(), createTraceable() };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces
            = { kv.first.getTrace(), kv.second.getTrace() };

    auto map = EvictingCacheMap<Traceable, Traceable>(1);
    map.put(move(kv.first), move(kv.second));

    map = map;

    ASSERT_LE(kvTraces.first.getCopyCalls(), 0);
    ASSERT_LE(kvTraces.second.getCopyCalls(), 0);
    ASSERT_TRUE(kvTraces.first.isAlive());
    ASSERT_TRUE(kvTraces.second.isAlive());
}

TEST_F(EvictingCacheMapTest, CopyAssignmentOther) {
    pair<Traceable, Traceable> kv[] = {
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

    ASSERT_LE(kvTraces[0].first.getCopyCalls(), 0);
    ASSERT_LE(kvTraces[0].second.getCopyCalls(), 0);
    ASSERT_FALSE(kvTraces[0].first.isAlive());
    ASSERT_FALSE(kvTraces[0].second.isAlive());

    ASSERT_LE(kvTraces[1].first.getCopyCalls(), 1);
    ASSERT_LE(kvTraces[1].second.getCopyCalls(), 1);
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

    ASSERT_THAT(map0, ::testing::ElementsAreArray(map1.begin(), map1.end()));
}

//  move assignment

TEST_F(EvictingCacheMapTest, MoveAssignmentSelf) {
    pair<Traceable, Traceable> kv = { createTraceable(), createTraceable() };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces
            = { kv.first.getTrace(), kv.second.getTrace() };

    auto map = EvictingCacheMap<Traceable, Traceable>(1);
    map.put(move(kv.first), move(kv.second));

    map = std::move(map);

    ASSERT_LE(kvTraces.first.getCopyCalls(), 0);
    ASSERT_LE(kvTraces.second.getCopyCalls(), 0);
    ASSERT_TRUE(kvTraces.first.isAlive());
    ASSERT_TRUE(kvTraces.second.isAlive());
}

TEST_F(EvictingCacheMapTest, MoveAssignmentOther) {
    pair<Traceable, Traceable> kv[] = {
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

    map0 = std::move(map1); // +1 destructor [0][k, v]

    ASSERT_LE(kvTraces[0].first.getCopyCalls(), 0);
    ASSERT_LE(kvTraces[0].second.getCopyCalls(), 0);
    ASSERT_FALSE(kvTraces[0].first.isAlive());
    ASSERT_FALSE(kvTraces[0].second.isAlive());

    ASSERT_LE(kvTraces[1].first.getCopyCalls(), 0);
    ASSERT_LE(kvTraces[1].second.getCopyCalls(), 0);
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
    ASSERT_EQ(map1.size(), 10u);

    ASSERT_THAT(map1, ::testing::ElementsAreArray(vec.begin(), vec.end()));
}

//  exists

TEST_F(EvictingCacheMapTest, Exists) {
    auto map = EvictingCacheMap<int, int>(2);
    map.put(1, 2);
    map.put(3, 4);

    map.get(3);     //  put {3, 4} on the top of the queue
    map.put(5, 6);  //  evict {1, 2}

    ASSERT_FALSE(map.exists(1));
    ASSERT_TRUE(map.exists(3));
}

//  find

TEST_F(EvictingCacheMapTest, Find) {
    auto map = EvictingCacheMap<int, int>(2);
    map.put(1, 2);
    map.put(3, 4);

    auto it = map.find(3);  //  put {3, 4} on the top of the queue
    ASSERT_EQ(it->second, 4);

    map.put(5, 6);  //  evict {1, 2}

    it = map.find(1);
    ASSERT_EQ(it, map.end());
}

//  erase

TEST_F(EvictingCacheMapTest, Erase) {
    auto map = EvictingCacheMap<int, int>(2);

    map.put(1, 2);
    ASSERT_TRUE(map.erase(1));

    map.put(3, 4);
    ASSERT_FALSE(map.erase(8));

    map.put(5, 6);
    ASSERT_TRUE(map.erase(5));

    ASSERT_FALSE(map.exists(1));
    ASSERT_TRUE(map.exists(3));
    ASSERT_FALSE(map.exists(5));
    ASSERT_EQ(map.size(), 1u);
}

//  empty, clear, size

TEST_F(EvictingCacheMapTest, EmptyClear) {
    auto map = EvictingCacheMap<int, int>(2);
    ASSERT_TRUE(map.empty());
    ASSERT_EQ(map.size(), 0u);

    map.put(1, 2);
    ASSERT_FALSE(map.empty());
    ASSERT_EQ(map.size(), 1u);

    map.clear();
    ASSERT_TRUE(map.empty());
    ASSERT_EQ(map.size(), 0u);
}

// iterator

TEST_F(EvictingCacheMapTest, Iterator) {
    auto map = EvictingCacheMap<int, int>(5);
    ASSERT_TRUE(map.begin() == map.end());

    map.put(1, 2);
    map.put(3, 4);
    map.put(5, 6);
    map.put(7, 8);
    map.get(1);     //  4
    map.get(3);     //  3
    map.get(5);     //  2
    map.get(7);     //  1

    auto && it = map.begin();
    ASSERT_EQ((*it).first, 7);
    ASSERT_EQ(it->second, 8);

    auto && tmp = it++;
    ASSERT_TRUE(tmp != it);
    ASSERT_EQ((*tmp).first, 7);
    ASSERT_EQ(tmp->second, 8);
    ASSERT_EQ((*it).first, 5);
    ASSERT_EQ(it->second, 6);

    tmp = ++it;
    ASSERT_TRUE(tmp == it);
    ASSERT_EQ((*it).first, 3);
    ASSERT_EQ(it->second, 4);

    ++it;
    ASSERT_EQ((*it).first, 1);
    ASSERT_EQ(it->second, 2);

    ++it;
    ASSERT_TRUE(it == map.end());
}

TEST_F(EvictingCacheMapTest, IteratorShuffle) {
    auto map = EvictingCacheMap<int, int>(5);

    map.put(0, 0);
    map.put(1, 1);
    map.put(2, 2);
    map.get(2);
    map.get(0);
    map.get(2); //  3
    map.get(1); //  2
    map.get(0); //  1

    auto it = map.begin();

    ASSERT_EQ(0, (it++)->first);
    ASSERT_EQ(1, (it++)->first);
    ASSERT_EQ(2, (it++)->first);
}

//  const_iterator

TEST_F(EvictingCacheMapTest, ConstIterator) {
    auto __map = EvictingCacheMap<int, int>(5);
    const auto & map = __map;
    ASSERT_TRUE(map.begin() == map.end());

    __map.put(1, 2);
    __map.put(3, 4);
    __map.put(5, 6);
    __map.put(7, 8);
    __map.get(1);   //  4
    __map.get(3);   //  3
    __map.get(5);   //  2
    __map.get(7);   //  1

    auto && it = map.begin();
    ASSERT_EQ((*it).first, 7);
    ASSERT_EQ(it->second, 8);

    auto && tmp = it++;
    ASSERT_TRUE(tmp != it);
    ASSERT_EQ((*tmp).first, 7);
    ASSERT_EQ(tmp->second, 8);
    ASSERT_EQ((*it).first, 5);
    ASSERT_EQ(it->second, 6);

    tmp = ++it;
    ASSERT_TRUE(tmp == it);
    ASSERT_EQ((*it).first, 3);
    ASSERT_EQ(it->second, 4);

    ++it;
    ASSERT_EQ((*it).first, 1);
    ASSERT_EQ(it->second, 2);

    ++it;
    ASSERT_TRUE(it == map.end());

    ASSERT_TRUE(map.cbegin() == map.begin());
    ASSERT_TRUE(map.cend() == map.end());
}

//  zero capacity

TEST_F(EvictingCacheMapTest, ZeroCapacity) {
    pair<Traceable, Traceable> kv = { createTraceable(), createTraceable() };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces
            = { kv.first.getTrace(), kv.second.getTrace() };

    auto map = EvictingCacheMap<Traceable, Traceable>(0);
    map.put(kv.first, kv.second);

    ASSERT_LE(kvTraces.first.getCopyCalls(), 0);
    ASSERT_LE(kvTraces.second.getCopyCalls(), 0);
    ASSERT_TRUE(kvTraces.first.isAlive());
    ASSERT_TRUE(kvTraces.second.isAlive());

    ASSERT_EQ(map.find(kv.first), map.end());
    ASSERT_FALSE(map.exists(kv.first));
    ASSERT_FALSE(map.erase(kv.first));
}

//  rehash

template <class T>
struct BadHash {

    size_t operator()(const T &) const {
        return 0;
    }
};

TEST_F(EvictingCacheMapTest, Rehash) {
    auto map = EvictingCacheMap<int, int, BadHash<int>>(10);
    auto l = list<pair<const int, int>>();

    for (int i = 0; i < 10; ++i) {
        map.put(i, i);
        l.emplace_front(i, i);
    }

    ASSERT_THAT(map, ::testing::ElementsAreArray(l.begin(), l.end()));
}

TEST_F(EvictingCacheMapTest, RehashTraceable) {
    pair<Traceable, Traceable> kv[] = {
            { createTraceable(), createTraceable() },
            { createTraceable(), createTraceable() }
    };
    pair<const Traceable::Trace &, const Traceable::Trace &> kvTraces[] = {
            { kv[0].first.getTrace(), kv[0].second.getTrace() },
            { kv[1].first.getTrace(), kv[1].second.getTrace() }
    };

    auto map = EvictingCacheMap<Traceable, Traceable, BadHash<Traceable>>(1);
    map.put(move(kv[0].first), move(kv[0].second));
    map.put(move(kv[1].first), move(kv[1].second));

    ASSERT_LE(kvTraces[0].first.getCopyCalls(), 0);
    ASSERT_LE(kvTraces[0].second.getCopyCalls(), 0);
    ASSERT_LE(kvTraces[1].first.getCopyCalls(), 0);
    ASSERT_LE(kvTraces[1].second.getCopyCalls(), 0);
}