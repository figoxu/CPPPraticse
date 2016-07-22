cmake_minimum_required(VERSION 3.5)
project(hello)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
message(STATUS "src dir = ${hello_SOURCE_DIR}")
include_directories("./redis-cpp")
link_directories("./redis-cpp")
set(SOURCE_FILES
        ./redis-cpp/anet.o
        ./redis-cpp/redis_command.o
        ./redis-cpp/redis_connection.o
        ./redis-cpp/redis_error.o
        ./redis-cpp/redis_pool.o
        ./redis-cpp/redis_utils.o
        main.cpp)
add_executable(redisCppDemo ${SOURCE_FILES} ./redis-cpp)