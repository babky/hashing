cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

SET(CMAKE_CXX_FLAGS "-O3 -std=gnu++0x")

find_package( Boost 1.34 COMPONENTS random thread REQUIRED )

link_directories ( ${Boost_LIBRARY_DIRS} )

set(SOURCES
    ../utils/storage_statistics.cpp
    ../utils/hash_math.cpp
    ../utils/static_random_generator.cpp
    ../utils/hash_assert.cpp
    ../policies/rehash_policy.cpp
    ../policies/guaranteed_rehash_policy.cpp
    ../policies/worst_case_rehash_policy.cpp
    ../storage.cpp
    ../math/double_word.cpp
)

include_directories ( .. ${Boost_INCLUDE_DIRS} )

add_library (Table STATIC ${SOURCES})

target_link_libraries (
    Table
    ${Boost_LIBRARIES}
)
