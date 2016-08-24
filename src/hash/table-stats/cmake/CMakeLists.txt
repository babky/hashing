cmake_minimum_required( VERSION 2.8.1 FATAL_ERROR )

find_package( Boost 1.34 COMPONENTS random thread program_options system REQUIRED )

link_directories ( ${Boost_LIBRARY_DIRS} )

set(SOURCES 
  ../table_stats.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  ../../test-suite
  ../../table-lib
)

link_directories ( ${CMAKE_CURRENT_SOURCE_DIR}/../../test-suite/cmake ${CMAKE_CURRENT_SOURCE_DIR}/../../table-lib/cmake ${Boost_LIBRARY_DIRS} )

add_executable ( TableStats ${SOURCES} )

target_link_libraries( TableStats TestSuite Table ${Boost_LIBRARIES} )
