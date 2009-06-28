#include "table_test.h"
#include <functional>
#include <algorithm>

using namespace Hash;
using namespace Hash::Tests;
using namespace Hash::Utils;
using namespace std;

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
	this->testStatistics();
	this->simpleRehash();
	this->testCopyCtor();
	this->testMassiveFill();
}

void TableTest::fillData(TableType & t) {
	this->assertEqual(0, t.getSize(), "Empty table must have just 0 items.");
	t.insert(3);
	this->assertEqual(1, t.getSize(), "Table (after t.insert(3)) should contain only 1 element.");
	this->assertException<std::exception>(InsertOperation<int, TableType>(3, t), "Inserting 3 causes exception.");
	this->assertTrue(t.contains(3), "Table should contain item 3.");
	this->assertFalse(t.contains(0), "Table should not contain item 0.");
	this->assertTrue(t.remove(3), "Removal must be successful.");
	this->assertFalse(t.contains(3), "Table should not contain item 3.");
}

void TableTest::testStatistics(void) {
	TableType t;
	this->fillData(t);
	StorageStatistics stats = StorageStatistics();
	t.computeStatistics(stats);
	this->assertEqual(t.getLoadFactor(), stats.getLoadFactor(), "Load factors must be equal (1).");
	t.insert(3);
	this->assertEqual<size_t>(16, stats.getTableLength(), "Default table length is sufficient.");

	stats.clear();
	t.computeStatistics(stats);
	this->assertEqual(t.getLoadFactor(), stats.getLoadFactor(), "Load factors must be equal (2).");
}

void TableTest::simpleRehash(void) {
	TableType t;
	t.rehash();
	t.insert(3);
	t.rehash();
	t.insert(4);
	t.rehash();
}

void TableTest::testCopyCtor(void) {
	TableType t;
	this->fillData(t);
	TableType b;
	b = t;
}

void TableTest::testMassiveFill(void) {
	TableType t;
	for (int i = 0; i < 50000; ++i) {
		t.insert(i);
	}

	this->assertEqual(50000, t.getSize(), "After inserting n distinct elements we expect table length of n.");
}
