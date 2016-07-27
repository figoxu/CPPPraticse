cmake_minimum_required(VERSION 3.5)
project(cSample2)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set_source_files_properties(lib1 lib2 ./)



include_directories(lib1 lib2)
link_directories(lib1 lib2)

set(SOURCE_FILES ./lib1/Hello.c main.cpp lib2/World.h lib2/World.c)
add_executable(cSample2 ${SOURCE_FILES})