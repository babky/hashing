cmake_minimum_required	(VERSION 2.8.1)
cmake_policy			(VERSION 2.8.1)

find_package( Boost 1.34 COMPONENTS random )

SET(CMAKE_CXX_FLAGS_DEBUG "-std=gnu++17 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=gnu++17 -O3")

set(SOURCES
    ../test.cpp
)

include_directories ( .. ../../table-lib ${Boost_INCLUDE_DIRS} )

link_directories ( ${CMAKE_CURRENT_SOURCE_DIR}/../../table-lib/cmake ${Boost_LIBRARY_DIRS} )

add_executable ( TableLengthTest ${SOURCES} )

target_link_libraries (
    TableLengthTest
    Table
    ${Boost_LIBRARIES}
)
