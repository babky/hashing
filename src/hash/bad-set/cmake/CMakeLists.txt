cmake_minimum_required (VERSION 2.8.1)
cmake_policy (VERSION 2.8.1)

find_package(Boost 1.34 COMPONENTS random program_options system thread date_time REQUIRED)
find_package(Paradiseo COMPONENTS eo moeo eoutils REQUIRED)

SET(CMAKE_CXX_FLAGS_DEBUG "-std=gnu++0x -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=gnu++0x -O3")

set(SOURCES 
  ../bad_set.cpp
)

set(HEADERS
  ../../table-lib
  ${Boost_INCLUDE_DIRS}
  ${PARADISEO_INCLUDE_DIR}
)

include_directories( 
  ../../table-lib
  ${Boost_INCLUDE_DIRS}
  ${PARADISEO_INCLUDE_DIR}
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../table-lib/cmake ${Boost_LIBRARY_DIRS})

add_executable(BadSet ${SOURCES})

target_link_libraries(BadSet Table ${Boost_LIBRARIES} ${PARADISEO_LIBRARIES})
