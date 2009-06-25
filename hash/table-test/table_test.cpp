#include "table_test.h"
#include "table.h"
#include "function.h"

using namespace Hash;
using namespace Hash::Tests;

TableTest::TableTest(void)
{
}

void TableTest::runTest(void)
{
	Table<int, Function> t;

	this->assertEqual(0, t.size());
	t.insert(3);
	t.size();
	this->assertEqual(1, t.size());
}
