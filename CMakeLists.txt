cmake_minimum_required(VERSION 3.3)
project(pps_team_kernel_panic)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    done/week04/db_list.c
    done/week04/db_utils.c
    done/week04/error.c
    done/week04/error.h
    done/week04/pictDB.h
    done/week04/pictDBM.c)

add_executable(pps_team_kernel_panic ${SOURCE_FILES})