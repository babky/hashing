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
		typedef Hash::Table<size_t, Hash::Utils::EqualityComparer<size_t>, Hash::UniversalFunctionCWLF, 
			Hash::Storages::ChainedStorage> LengthTestTable;
//		typedef Hash::Table<size_t, Hash::Utils::EqualityComparer<size_t>, Hash::UniversalFunctionLinearMap, 
//			Hash::Storages::ChainedStorage> LengthTestTable;

		void runTest(void);
	};

} }

#endif /* TEST_H */