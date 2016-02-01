cmake_minimum_required(VERSION 3.3)
project(gfytube_render)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
set(SOURCE_FILES ${SOURCE_FILES} json.hpp rapidxml.hpp duktape.c duktape.h md5.cpp md5.h)
set(SOURCE_FILES ${SOURCE_FILES} audio.cpp audio.h video.cpp video.h helpers.cpp helpers.h)


set(USE_SYSTEM_CURL ON)
add_subdirectory(deps/cpr)

add_executable(gfytube_render ${SOURCE_FILES})

include_directories(${CPR_INCLUDE_DIRS})
include_directories(deps/libdash/libdash/libdash/include)

target_link_libraries(gfytube_render ${CPR_LIBRARIES})