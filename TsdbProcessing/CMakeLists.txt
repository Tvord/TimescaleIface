project(tsdb_process)
cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(TSDBConnector /usr/include/pgsql/)
add_subdirectory(TSDBConnector)

add_executable(tsdb_process example/main.cpp)
target_link_libraries(tsdb_process tsdbconnector)

add_executable(tsdb_massInsert example/mainMassive.cpp)
target_link_libraries(tsdb_massInsert tsdbconnector)
