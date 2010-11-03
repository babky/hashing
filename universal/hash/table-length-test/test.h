#ifndef TEST_H
#define TEST_H

#include "table.h"
#include "function.h"
#include "universal_systems.h"
#include "storage.h"
#include "storages/chained_storage.h"
#include "utils/equality_comparer.h"

namespace Hash { namespace Tests {

	class LengthTest {
	public:
		typedef Hash::Table<int, Hash::Utils::EqualityComparer<int>, Hash::Function, Hash::Storages::ChainedStorage> 
			SimpleTable;

		void runTest(void);
	};

} }

#endif /* TEST_H */