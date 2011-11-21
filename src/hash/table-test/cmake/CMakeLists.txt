cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

find_package( Boost 1.34 COMPONENTS random system date_time REQUIRED )

SET(CMAKE_CXX_FLAGS "-std=gnu++0x -ggdb")

set(SOURCES 
  ../test.cpp
)

include_directories( 
  ../../table-lib
  ../../test-suite
  ${Boost_INCLUDE_DIRS}
)

link_directories ( ${CMAKE_CURRENT_SOURCE_DIR}/../../test-suite/cmake ${CMAKE_CURRENT_SOURCE_DIR}/../../table-lib/cmake ${Boost_LIBRARY_DIRS} )

add_executable ( TableTest ${SOURCES} )

target_link_libraries( TableTest TestSuite Table )
