cmake_minimum_required(VERSION 3.12)
project(server)
set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11 -Wall")
include_directories(.)
include_directories(../lib)
include_directories(../proto)
add_executable(server
        server.c
        server.h
        ../proto/proto.h
        main.c ../proto/proto.c ../common/ccommon.h ../common/ccommon.c)
