#include "storages/collision_count_storage.h"
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

int main(int, const char **) {
	using namespace Hash;
	using namespace Hash::Utils;
	using namespace std;

	Table<int, ConstantComparer<int>, Function, Hash::Storages::CollisionCountStorage> t(32);
	t.insert(1);
	t.insert(1);
	t.insert(1);
	t.insert(1);
	t.insert(1);

	StorageStatistics s;
	t.computeStatistics(s);
	std::cout << s;

	return 0;
}