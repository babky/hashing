set(SOURCES 
  ../table_test.cpp
  ../test.cpp
)

include_directories( 
  ../../table-lib
  ../../test-suite
)

link_directories (../../test-suite/cmake)
link_directories (../../table-lib/cmake)

target_link_libraries( TestSuite Table )

add_library (TestSuite STATIC ${SOURCES})
