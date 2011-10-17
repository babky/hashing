cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

find_package( Boost 1.34 COMPONENTS random system date_time REQUIRED )

set(SOURCES 
  ../system_test.cpp
)

include_directories(
  ../../table-lib
)

link_directories (../../table-lib/cmake)

add_executable ( SystemTest ${SOURCES} )

target_link_libraries( SystemTest Table )
