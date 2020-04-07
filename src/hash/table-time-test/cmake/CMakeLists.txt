cmake_minimum_required (VERSION 3.17.0)
cmake_policy           (VERSION 3.17.0)

find_package(Boost 1.72.0 COMPONENTS random system date_time program_options REQUIRED)

SET(CMAKE_CXX_FLAGS_DEBUG "-std=gnu++17 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=gnu++17 -O3 -gtoggle -ftree-vectorize -msse2")

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
