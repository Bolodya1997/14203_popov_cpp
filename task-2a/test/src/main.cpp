#include <gtest/gtest.h>

#include <vector>

#include <Stream.h>
#include <map.h>
#include <get.h>
#include <filter.h>
#include <skip.h>
#include <group.h>
#include <Terminators.h>

#include "Noizy.h"

using std::cout;
using std::endl;
using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::to_string;

using ::map;
using ::get;

int main(int argc, char * * argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

