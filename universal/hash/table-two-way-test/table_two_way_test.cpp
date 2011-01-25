#include "storages/collision_count_storage.h"
#include "systems/two_way_system.h"
#include "systems/cwlf_system.h"
#include "systems/linear_map_system.h"
#include "table.h"
#include "function.h"
#include "universal_systems.h"
#include "storage.h"
#include "policies/guaranteed_rehash_policy.h"
#include "storages/chained_storage.h"
#include "storages/bounded_chained_storage.h"
#include "systems/linear_map_system.h"
#include "utils/equality_comparer.h"
#include "utils/storage_statistics.h"
#include "utils/constant_comparer.h"
#include "default_test.h"
#include <iostream>

using namespace Hash;
using namespace Hash::Utils;
using namespace Hash::Systems;
using namespace std;

template <typename T>
class TwoWaySystemCWLF : public Hash::Systems::TwoWaySystem<T, Hash::UniversalFunctionCWLF> {
};

template <typename T>
class TwoWaySystemLinearMap : public Hash::Systems::TwoWaySystem<T, Hash::UniversalFunctionLinearMap> {
};

int main(int, const char **) {
	const size_t TEST_LENGTH = 1 << 24;
	typedef boost::uint_fast64_t ValueType;
	RandomGenerator<ValueType> generator(0, boost::integer_traits<ValueType>::const_max, true);
	Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemLinearMap, Hash::Storages::CollisionCountStorage> t(TEST_LENGTH);
	
	for (size_t i = 0; i < TEST_LENGTH; ++i) {
		t.insert(generator.generate());

		/*if (0 == i % 1000) {
			StorageStatistics s;
			t.computeStatistics(s);
			std::cout << s;
		}*/
	}

	StorageStatistics s;
	t.computeStatistics(s);
	std::cout << s;
	return 0;
}