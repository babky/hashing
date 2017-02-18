cmake_minimum_required	(VERSION 2.8.1)
cmake_policy			(VERSION 2.8.1)

find_package( Boost 1.34 COMPONENTS random program_options system thread date_time REQUIRED )

SET(CMAKE_CXX_FLAGS_DEBUG "-std=gnu++17 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=gnu++17 -O3")

set(SOURCES 
  ../m_test.cpp
)

include_directories( 
  ../../table-lib
  ../../test-suite
  ${Boost_INCLUDE_DIRS}
)

link_directories ( ${CMAKE_CURRENT_SOURCE_DIR}/../../test-suite/cmake ${CMAKE_CURRENT_SOURCE_DIR}/../../table-lib/cmake ${Boost_LIBRARY_DIRS} )

add_executable ( MTest ${SOURCES} )

target_link_libraries( MTest TestSuite Table ${Boost_LIBRARIES} )
