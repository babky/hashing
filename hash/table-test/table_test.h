#ifndef TABLE_TEST_H
#define TABLE_TEST_H

#include "table.h"
#include "default_test.h"

namespace Hash { namespace Tests {

	class TableTest : public TestSuite::DefaultTest
	{
	public:
		TableTest(void);

		virtual void runTest(void);
	};

} }

#endif /* TABLE_TEST_H */
