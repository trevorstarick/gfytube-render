cmake_minimum_required(VERSION 2.8)
project(gfytube-cli)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
set(SOURCE_FILES ${SOURCE_FILES} json.hpp rapidxml.hpp duktape.c duktape.h md5.cpp md5.h)
set(SOURCE_FILES ${SOURCE_FILES} audio.cpp audio.h video.cpp video.h helpers.cpp helpers.h)

set(USE_SYSTEM_CURL ON)
add_subdirectory(deps/cpr)

find_package(Threads)

add_executable(gfytube-cli ${SOURCE_FILES})

include_directories(${CPR_INCLUDE_DIRS})
include_directories(deps/libdash/libdash/libdash/include)

target_link_libraries(gfytube-cli ${CPR_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_custom_command(TARGET gfytube-cli
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gfytube-cli> /Users/trevorstarick/builds/gfytube-cli/gfytube-cli)