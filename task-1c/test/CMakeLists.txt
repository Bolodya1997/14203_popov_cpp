file(GLOB SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${GTEST_INCLUDE})

add_executable(test_lru ${SRCS} ../include/EvictingCacheMap.h)

target_link_libraries(test_lru
        ${GTEST_LIB})

install(TARGETS test_lru
        DESTINATION .)

# coverage
set(TEST_TARGET test_lru)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CodeCoverage)
    setup_target_for_coverage(
            NAME ${PROJECT_NAME}_coverage
            EXECUTABLE ${TEST_TARGET})

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
endif() #CMAKE_BUILD_TYPE STREQUAL "Coverage"