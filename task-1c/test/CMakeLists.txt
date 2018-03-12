file(GLOB SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${GTEST_INCLUDE})

set(TEST_TARGET test_lru)
add_executable(${TEST_TARGET} ${SRCS} ../include/EvictingCacheMap.h ../include/EvictingCacheMap.inc)

target_link_libraries(test_lru
        ${GTEST_LIB})

install(TARGETS test_lru
        DESTINATION .)

# coverage
if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
    include(CodeCoverage)

    setup_target_for_coverage(
            NAME ${PROJECT_NAME}_coverage
            EXECUTABLE ${TEST_TARGET})

    append_coverage_compiler_flags()
endif() #CMAKE_BUILD_TYPE STREQUAL "Coverage"