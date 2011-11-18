cmake_minimum_required( VERSION 2.8.1 FATAL_ERROR )

find_package( Boost 1.34 COMPONENTS random thread program_options system REQUIRED )

link_directories ( ${Boost_LIBRARY_DIRS} )

set(SOURCES 
  ../table_two_way_test.cpp
  ../tests/test.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  ../../table-lib
  ../../test-suite
)

link_directories ( ../../test-suite/cmake ../../table-lib/cmake link_directories ${Boost_LIBRARY_DIRS} )

add_executable ( TableTwoWayTest ${SOURCES} )

target_link_libraries( TableTwoWayTest TestSuite Table ${Boost_LIBRARIES} )
