cmake_minimum_required (VERSION 3.17.0)
cmake_policy           (VERSION 3.17.0)

find_package(Boost 1.72.0 COMPONENTS random program_options system thread date_time REQUIRED)

SET(CMAKE_CXX_FLAGS_DEBUG "-std=gnu++0x -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=gnu++0x -O3")

set(SOURCES
  ../surjectivity.cpp
)

include_directories( 
  ../../table-lib
  ${Boost_INCLUDE_DIRS}
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../table-lib/cmake ${Boost_LIBRARY_DIRS})

add_executable(Surjectivity ${SOURCES})

target_link_libraries(Surjectivity Table ${Boost_LIBRARIES})
