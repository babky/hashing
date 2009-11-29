cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

find_package( Boost 1.34 COMPONENTS random )

link_directories ( ${Boost_LIBRARY_DIRS} )

set(SOURCES
    ../utils/storage_statistics.cpp
    ../utils/static_random_generator.cpp
    ../policies/rehash_policy.cpp
    ../policies/worst_case_rehash_policy.cpp
    ../storage.cpp
)

include_directories ( .. ${Boost_INCLUDE_DIRS} )

add_library (Table STATIC ${SOURCES})

target_link_libraries (
    Table
    ${Boost_LIBRARIES}
)
