#include "table_test.h"
#include "table.h"
#include "function.h"
#include "storage.h"
#include "utils/equality_comparer.h"
#include <functional>
#include <algorithm>

using namespace Hash;
using namespace Hash::Tests;
using namespace Hash::Utils;
using namespace std;

typedef Table<int, EqualityComparer<int>, Function, ChainedStorage> TableType;

template<class T, class Table>
class InsertOperation {
public:
	InsertOperation(const T & argument, Table & table):
	  table(&table),
	  argument(argument)
	{
	}

	void operator()(void) {
		this->table->insert(this->argument);
	}

	Table * table;
	T argument;
};

TableTest::TableTest(void)
{
}

void TableTest::runTest(void)
{
	TableType t;
	this->assertEqual(0, t.getSize(), "Empty table must have just 0 items.");
	t.insert(3);
	this->assertEqual(1, t.getSize(), "Table (after t.insert(3)) should contain only 1 element.");
	this->assertException<std::exception>(InsertOperation<int, TableType>(3, t), "Inserting 3 again causes exception.");
	this->assertTrue(t.contains(3), "Table should contain item 3.");
	this->assertFalse(t.contains(0), "Table should not contain item 0.");
	this->assertTrue(t.remove(3), "Removal must be successful.");
	this->assertFalse(t.contains(3), "Table should not contain item 3.");
}
