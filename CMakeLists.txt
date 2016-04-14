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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_REENTRANT -I/usr/local/Cellar/vips/8.2.3/include -I/usr/local/Cellar/libgsf/1.14.36/include/libgsf-1 -I/usr/local/Cellar/fftw/3.3.4_1/include -I/usr/local/Cellar/orc/0.4.25/include/orc-0.4 -I/usr/local/Cellar/little-cms2/2.7/include -I/usr/local/Cellar/pango/1.38.1/include/pango-1.0 -I/usr/local/Cellar/harfbuzz/1.2.6/include/harfbuzz -I/usr/local/Cellar/pango/1.38.1/include/pango-1.0 -I/usr/local/Cellar/fontconfig/2.11.1_2/include -I/usr/local/Cellar/freetype/2.6.3/include/freetype2 -I/usr/local/Cellar/libtiff/4.0.6/include -I/usr/local/Cellar/libpng/1.6.21/include/libpng16 -I/usr/local/Cellar/libexif/0.6.21/include -I/usr/local/Cellar/glib/2.46.2/include/glib-2.0 -I/usr/local/Cellar/glib/2.46.2/lib/glib-2.0/include -I/usr/local/opt/gettext/include -I/usr/include/libxml2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I/usr/local/opt/openssl/include")

file(GLOB SOURCE_FILES "done/pictDBM/*.h" "done/pictDBM/*.c")

add_executable(pps_team_kernel_panic ${SOURCE_FILES})
#target_link_libraries(pps_team_kernel_panic -lssl -lcrypto -L/usr/local/Cellar/vips/8.2.3/lib -L/usr/local/Cellar/glib/2.46.2/lib -L/usr/local/opt/gettext/lib -lvips -lgobject-2.0 -lglib-2.0 -lintl -lm)
