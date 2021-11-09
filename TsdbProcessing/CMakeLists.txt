project(tsdb_process)
cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(TSDBConnector /usr/include/pgsql/)
add_subdirectory(TSDBConnector)
add_subdirectory(iTSDB)

find_package(Qt5 COMPONENTS Core)
ADD_COMPILE_OPTIONS("-pg")
ADD_LINK_OPTIONS(-pg)

add_executable(tsdb_process example/main.cpp)
target_link_libraries(tsdb_process tsdbconnector)

add_executable(tsdb_massInsert example/mainMassive.cpp)
target_link_libraries(tsdb_massInsert tsdbconnector)

add_executable(tsdb_interface example/mainIface.cpp)
target_link_libraries(tsdb_interface tsdbconnector itsdb)

add_executable(strTest example/stringTest.cpp)
target_link_libraries(strTest Qt5::Core)

