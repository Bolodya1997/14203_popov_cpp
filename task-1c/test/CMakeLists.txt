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
