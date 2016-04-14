cmake_minimum_required(VERSION 3.5)
project(done)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    pictDBM/db_create.c
    pictDBM/db_delete.c
    pictDBM/db_list.c
    pictDBM/db_utils.c
    pictDBM/error.c
    pictDBM/error.h
    pictDBM/Makefile
    pictDBM/pictDB.h
    pictDBM/pictDBM.c
    pictDBM/testDB01.pictdb_static
    pictDBM/testDB02.pictdb_static)

add_executable(done ${SOURCE_FILES})