cmake_minimum_required (VERSION 3.17.0)
cmake_policy           (VERSION 3.17.0)

SET(CMAKE_CXX_FLAGS_DEBUG "-std=gnu++17 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=gnu++17 -O3")

set(SOURCES
  ../test_assert.cpp
  ../default_test.cpp
  ../test.cpp
)

add_library(TestSuite STATIC ${SOURCES})
