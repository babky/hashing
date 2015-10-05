cmake_minimum_required	(VERSION 2.8.1)
cmake_policy			(VERSION 2.8.1)

set(SOURCES
  ../test_assert.cpp
  ../default_test.cpp
  ../test.cpp
)

add_library (TestSuite STATIC ${SOURCES})
