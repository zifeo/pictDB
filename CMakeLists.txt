cmake_minimum_required(VERSION 3.4)
project(pps_team_kernel_panic)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wfloat-equal")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wundef")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wshadow")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-prototypes")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wwrite-strings")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-qual")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wswitch-default")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wswitch-enum")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wconversion")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunreachable-code")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I/usr/local/opt/openssl/include")

execute_process(COMMAND pkg-config vips --cflags OUTPUT_VARIABLE VIPS_CFLAG)
#execute_process(COMMAND pkg-config vips ----libs OUTPUT_VARIABLE VIPS_LIBS)
#LINK_DIRECTORIES(${VIPS_LIBS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${VIPS_CFLAG}")

file(GLOB SOURCE_FILES "done/pictDBM/*.h" "done/pictDBM/*.c")

add_executable(pps_team_kernel_panic ${SOURCE_FILES})
