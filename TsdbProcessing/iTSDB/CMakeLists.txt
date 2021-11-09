project(itsdb)
cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 COMPONENTS Core)

ADD_COMPILE_OPTIONS("-pg")
ADD_LINK_OPTIONS(-pg)

include_directories(/usr/include/pgsql/)

add_library(itsdb STATIC TsdbInterface.h TsdbInterface.cpp)
target_link_libraries(itsdb Qt5::Core pq pthread)