cmake_minimum_required(VERSION 3.27)
project(orderbook)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(orderbook
        dll.hpp
        orderbook.cpp
        structures.cpp
        test.cpp)
