cmake_minimum_required	(VERSION 2.8.1)
cmake_policy			(VERSION 2.8.1)

SET(CMAKE_CXX_FLAGS_DEBUG "-std=gnu++0x -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=gnu++0x -O3")

find_package( Boost 1.34 COMPONENTS random thread system REQUIRED )

link_directories ( ${Boost_LIBRARY_DIRS} )

set(HEADERS 
	../function.h
	../storage.h
	../table.h
	../universal_systems.h
	../exceptions/item_stored_exception.h
	../generators/generator.h
	../generators/linear_generator.h
	../generators/random_generator.h
	../generators/unit_input_generator.h
	../iterators/set_iterator.h
	../experiments/longest_chain.h
	../experiments/surjectivity.h
	../experiments/traits.h
	../math/double_word.h
	../math/combinatorics.h
	../miscellaneous/linear_probing_table_implementation.h
	../policies/guaranteed_rehash_policy.h
	../policies/rehash_policy.h
	../policies/worst_case_rehash_policy.h
	../storages/bounded_chained_storage.h
	../storages/chained_storage.h
	../storages/collision_count_storage.h
	../storages/direct_chaining_storage.h
	../storages/probing_storage.h
	../storages/probing/policies.h
	../systems/bit_string_system.h
	../systems/bad_linear_system.h
	../systems/cwlf_exponential_system.h
	../systems/cwlf_system.h
	../systems/multiply_shift_system.h
	../systems/identity_function.h
	../systems/linear_map_system.h
	../systems/polynomial_system.h
	../systems/random_system.h
	../systems/tabulation_system.h
	../systems/tr1_function.h
	../systems/two_way_system.h
	../systems/two_way_system_randomized.h
	../systems/universal_system.h
	../systems/universal_system_factory.h
	../systems/uniform/dietzfelbinger_woelfel.h
	../systems/uniform/siegel.h
	../utils/boost_include.h
	../utils/config.h
	../utils/constant_comparer.h
	../utils/definitions.h
	../utils/equality_comparer.h
	../utils/hash_assert.h
	../utils/hash_math.h
	../utils/random_generator.h
	../utils/smart_pointer.h
	../utils/static_random_generator.h
	../utils/storage_statistics.h
	../utils/time_vector.h
	../utils/generators/generators.h
	../utils/generators/linear_generator.h
	../utils/generators/quadratic_generator.h
	../utils/generators/shift_generator.h
)

set(SOURCES
	${HEADERS}
	../utils/storage_statistics.cpp
	../utils/static_random_generator.cpp
	../utils/hash_assert.cpp
	../utils/time_vector.cpp
	../math/combinatorics.cpp
	../iterators/set_iterator.cpp
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
