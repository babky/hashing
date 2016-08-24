cmake_minimum_required	(VERSION 2.8.1)
cmake_policy			(VERSION 2.8.1)

find_package( Boost 1.59 COMPONENTS random program_options system thread date_time REQUIRED )

SET(CMAKE_CXX_FLAGS "-std=gnu++0x -ggdb -O3")

set(SOURCES 
  ../probability_distribution.cpp
)

include_directories( 
  ../../table-lib
  ${Boost_INCLUDE_DIRS}
)

link_directories ( ${CMAKE_CURRENT_SOURCE_DIR}/../../table-lib/cmake ${Boost_LIBRARY_DIRS} )

add_executable ( ProbabilityDistribution ${SOURCES} )

target_link_libraries( ProbabilityDistribution Table ${Boost_LIBRARIES} )
