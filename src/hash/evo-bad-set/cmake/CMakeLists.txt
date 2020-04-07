cmake_minimum_required (VERSION 3.17.0)
cmake_policy           (VERSION 3.17.0)

find_package(Boost 1.72.0 COMPONENTS random program_options system date_time REQUIRED)
find_package(Paradiseo COMPONENTS eo eoutils REQUIRED)

SET(CMAKE_CXX_FLAGS_DEBUG "-std=gnu++17 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=gnu++17 -O3")

set(SOURCES
  ../evo_bad_set.cpp
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

add_executable(EvoBadSet ${SOURCES})

target_link_libraries(EvoBadSet Table ${Boost_LIBRARIES} ${PARADISEO_LIBRARIES})
