cmake_minimum_required( VERSION 2.8.1 FATAL_ERROR )

find_package( Boost 1.34 COMPONENTS random thread program_options system REQUIRED )

SET(CMAKE_CXX_FLAGS_DEBUG "-std=gnu++17 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=gnu++17 -O3")

link_directories ( ${Boost_LIBRARY_DIRS} )

set(SOURCES 
  ../table_two_way_test.cpp
  ../tests/test.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  ../../table-lib
)

link_directories ( ${CMAKE_CURRENT_SOURCE_DIR}/../../table-lib/cmake ${Boost_LIBRARY_DIRS} )

add_executable ( TableTwoWayTest ${SOURCES} )

target_link_libraries( TableTwoWayTest Table ${Boost_LIBRARIES} )
