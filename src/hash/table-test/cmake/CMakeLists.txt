cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

find_package( Boost 1.34 COMPONENTS random system date_time REQUIRED )

set(SOURCES 
  ../test.cpp
)

include_directories( 
  ../../table-lib
  ../../test-suite
)

link_directories (../../test-suite/cmake  ${Boost_INCLUDE_DIRS} )
link_directories (../../table-lib/cmake ${Boost_LIBRARY_DIRS} )

add_executable ( TableTest ${SOURCES} )

target_link_libraries( TableTest TestSuite Table )
