cmake_minimum_required(VERSION 3.5)
project(cin_cout)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(cin_cout ${SOURCE_FILES})