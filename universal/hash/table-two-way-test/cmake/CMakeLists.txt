cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

set(SOURCES 
  ../two-way-test.cpp
)

include_directories( 
  ../../table-lib
  ../../test-suite
)

link_directories (../../test-suite/cmake)
link_directories (../../table-lib/cmake)

add_executable ( TableTwoWayTest ${SOURCES} )

target_link_libraries( TableTwoWayTest TestSuite Table )
