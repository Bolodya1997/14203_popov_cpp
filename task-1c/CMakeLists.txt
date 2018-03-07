cmake_minimum_required(VERSION 3.9)
project(LRU CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include_directories(${CMAKE_SOURCE_DIR}/include)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(test)