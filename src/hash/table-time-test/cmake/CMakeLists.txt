cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

find_package( Boost 1.34 COMPONENTS random system date_time REQUIRED )

set(SOURCES 
  ../table_time_test.cpp
)

include_directories(
  ../../table-lib
   ${Boost_INCLUDE_DIRS} 
)

link_directories (../../table-lib/cmake ${Boost_LIBRARY_DIRS} )

add_executable ( TableTimeTest ${SOURCES} )

target_link_libraries( TableTimeTest Table )
