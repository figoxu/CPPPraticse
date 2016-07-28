cmake_minimum_required(VERSION 3.5)
project(nginx_cpp_model)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set_source_files_properties( /src/core
        src/event/modules src/event/
        src/http/modules
        src/http/modules/perl
        src/http/v2
        src/http
        src/mail
        src/misc
        src/os/unix
        src/os/win32
        src/stream
        cpp/framework/epoll
        cpp/http/datahook
        cpp/http/echo
        cpp/http/filter
        cpp/http/loadbalance
        cpp/http/shmem
        cpp/http/subrequest
        cpp/http/test
        cpp/http/upstream
        cpp/http/variables
        cpp/ngxpp
        )


include_directories( /src/core
        src/event/modules src/event/
        src/http/modules
        src/http/modules/perl
        src/http/v2
        src/http
        src/mail
        src/misc
        src/os/unix
        src/os/win32
        src/stream
        cpp/framework/epoll
        cpp/http/datahook
        cpp/http/echo
        cpp/http/filter
        cpp/http/loadbalance
        cpp/http/shmem
        cpp/http/subrequest
        cpp/http/test
        cpp/http/upstream
        cpp/http/variables
        cpp/ngxpp
        )

link_directories( /src/core
        src/event/modules src/event/
        src/http/modules
        src/http/modules/perl
        src/http/v2
        src/http
        src/mail
        src/misc
        src/os/unix
        src/os/win32
        src/stream
        cpp/framework/epoll
        cpp/http/datahook
        cpp/http/echo
        cpp/http/filter
        cpp/http/loadbalance
        cpp/http/shmem
        cpp/http/subrequest
        cpp/http/test
        cpp/http/upstream
        cpp/http/variables
        cpp/ngxpp
        )



set(SOURCE_FILES main.cpp)
add_executable(nginx_cpp_model ${SOURCE_FILES})