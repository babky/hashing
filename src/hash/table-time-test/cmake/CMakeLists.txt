cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

SET(CMAKE_CXX_FLAGS "-std=gnu++0x -O3 -ggdb -march=native")

find_package( Boost 1.34 COMPONENTS random system date_time program_options REQUIRED )

set(SOURCES 
    ../table_time_test.cpp
)

include_directories(
    ../../table-lib
    ${Boost_INCLUDE_DIRS} 
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../table-lib/cmake
    ${Boost_LIBRARY_DIRS}
)

add_executable(TableTimeTest ${SOURCES})

target_link_libraries(TableTimeTest Table ${Boost_LIBRARIES})
