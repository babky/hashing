#ifndef TABLE_TEST_H
#define TABLE_TEST_H

#include "table.h"
#include "function.h"
#include "universal_function.h"
#include "storage.h"
#include "utils/equality_comparer.h"
#include "default_test.h"

namespace Hash { namespace Tests {

	class TableTest : public TestSuite::DefaultTest
	{
	public:
		TableTest(void);

		virtual void runTest(void);

	private:
		typedef Hash::Table<int, Hash::Utils::EqualityComparer<int>, Hash::UniversalFunctionCWLF, Hash::ChainedStorage> 
			TableType;

		virtual void fillData(TableType & t);
		virtual void testStatistics(void);
		virtual void simpleRehash(void);
		virtual void testCopyCtor(void);
		virtual void testMassiveFill(void);
	};

} }

#endif /* TABLE_TEST_H */
