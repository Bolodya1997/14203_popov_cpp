#include <iostream>
#include <unordered_map>

#include "EvictingCacheMap.h"

#include "Traceable.h"

using std::shared_ptr;

int main() {
    auto map = EvictingCacheMap<int, int>(10);
//    auto a = map.find(1);

    auto kek = map.cbegin();
    *kek;
}