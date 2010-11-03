#ifndef TEST_H
#define TEST_H

#include "table.h"
#include "function.h"
#include "systems/linear_map_system.h"
#include "universal_systems.h"
#include "storage.h"
#include "storages/chained_storage.h"
#include "utils/equality_comparer.h"

namespace Hash { namespace Tests {

	class LengthTest {
	public:
		typedef Hash::Table<int, Hash::Utils::EqualityComparer<int>, Hash::UniversalFunctionCWLF, 
			Hash::Storages::ChainedStorage> TableCWLF;
//		typedef Hash::Table<int, Hash::Utils::EqualityComparer<int>, Hash::UniversalFunctionLinearMap, 
//			Hash::Storages::ChainedStorage> TableCWLF;

		void runTest(void);
	};

} }

#endif /* TEST_H */