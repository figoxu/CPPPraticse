cmake_minimum_required(VERSION 3.5)
project(invokeC)
include_directories(cSample)
link_directories(cSample)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES cSample/Hello.o main.cpp)
add_executable(invokeC ${SOURCE_FILES})