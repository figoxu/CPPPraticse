cmake_minimum_required(VERSION 3.5)
project(extend)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(extend ${SOURCE_FILES})