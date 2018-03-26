cmake_minimum_required(VERSION 3.9)
project(Stream CXX)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake-modules)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -pedantic")

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(third-party)
add_subdirectory(test)
