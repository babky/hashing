cmake_minimum_required	(VERSION 2.8.1)
cmake_policy			(VERSION 2.8.1)

find_package(Boost 1.34 COMPONENTS random system date_time REQUIRED)

SET(CMAKE_CXX_FLAGS_DEBUG "-std=gnu++11 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=gnu++11 -O3")


include_directories(
  ../../table-lib
  ../../test-suite
  ${Boost_INCLUDE_DIRS}
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../test-suite/cmake ${CMAKE_CURRENT_SOURCE_DIR}/../../table-lib/cmake ${Boost_LIBRARY_DIRS})

add_executable(TableTest ../table_test.h ../test.cpp)
target_link_libraries(TableTest TestSuite Table)

add_executable(BadLinearSystemTest ../bad_linear_system_test.cpp)
target_link_libraries(BadLinearSystemTest TestSuite Table)


