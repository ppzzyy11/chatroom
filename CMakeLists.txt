cmake_minimum_required (VERSION 2.8)

# projectname is the same as the main-executable
project(chatroom CXX)


# set c++ compiler flags for GUN
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS " -pthread -lgtest -std=c++11 -Wall -levent ")
endif()

# project version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATRH 0)

# add subdirectory

add_subdirectory(src/client)
add_subdirectory(src/common)
add_subdirectory(src/server)
add_subdirectory(src/test)

