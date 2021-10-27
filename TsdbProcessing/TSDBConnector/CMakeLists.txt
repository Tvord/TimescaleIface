project(tsdbconnector)
cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 COMPONENTS Core)


include_directories(/usr/include/pgsql/)

add_library(tsdbconnector STATIC tsdbconnector.h tsdbconnector.cpp)
target_link_libraries(tsdbconnector Qt5::Core pq)