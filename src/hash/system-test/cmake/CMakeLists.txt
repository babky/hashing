cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

SET(CMAKE_CXX_FLAGS "-std=gnu++0x -O2 -gtoggle -ftree-vectorize -msse2")

find_package( Boost 1.34 COMPONENTS random program_options system date_time REQUIRED )

set(SOURCES 
  ../system_test.cpp
)

include_directories(
  ../../table-lib
   ${Boost_INCLUDE_DIRS} 
)

link_directories ( ${CMAKE_CURRENT_SOURCE_DIR}/../../table-lib/cmake ${Boost_LIBRARY_DIRS} )

add_executable ( SystemTest ${SOURCES} )

target_link_libraries( SystemTest Table ${Boost_LIBRARIES} )
