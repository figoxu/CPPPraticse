cmake_minimum_required(VERSION 3.5)
project(cSample)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c main.c Hello.h Hello.c)
add_executable(cSample ${SOURCE_FILES})