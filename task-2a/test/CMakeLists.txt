file(GLOB SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_SOURCE_DIR}/include/*.h)    #   force CLion to check these files

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${GTEST_INCLUDE})

set(TEST_TARGET test_stream)
add_executable(${TEST_TARGET} ${SRCS})

target_link_libraries(${TEST_TARGET}
        ${GTEST_LIB})

install(TARGETS		${TEST_TARGET}
        DESTINATION	.)

# coverage
if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
    include(CodeCoverage)

    setup_target_for_coverage(
            NAME	${PROJECT_NAME}_coverage
            EXECUTABLE	${TEST_TARGET})

    append_coverage_compiler_flags()
endif() #CMAKE_BUILD_TYPE STREQUAL "Coverage"
