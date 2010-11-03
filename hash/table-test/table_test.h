#ifndef TABLE_TEST_H
#define TABLE_TEST_H

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
#include "default_test.h"

namespace Hash { namespace Tests {

	template<class HashTable>
	class TableTest : public TestSuite::DefaultTest
	{
	public:
		TableTest(void) {
		}

		virtual void runTest(void) {
			this->testStatistics();
			this->simpleRehash();
			this->testCopyCtor();
			this->testMassiveFill();
		}

	private:
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

		typedef HashTable TableType;

		virtual void fillData(TableType & t) {
			this->assertEqual(0, t.getSize(), "Empty table must have just 0 items.");
			t.insert(3);
			this->assertEqual(1, t.getSize(), "Table (after t.insert(3)) should contain only 1 element.");
			this->assertException<std::exception>(InsertOperation<int, TableType>(3, t), "Inserting 3 causes exception.");
			this->assertTrue(t.contains(3), "Table should contain item 3.");
			this->assertFalse(t.contains(0), "Table should not contain item 0.");
			this->assertTrue(t.remove(3), "Removal must be successful.");
			this->assertFalse(t.contains(3), "Table should not contain item 3.");
		}

		virtual void testStatistics(void) {
			TableType t;
			this->fillData(t);
			Hash::Utils::StorageStatistics stats = Hash::Utils::StorageStatistics();
			t.computeStatistics(stats);
			this->assertEqual(t.getLoadFactor(), stats.getLoadFactor(), "Load factors must be equal (1).");
			t.insert(3);
			this->assertEqual<size_t>(16, stats.getTableLength(), "Default table length is sufficient.");

			stats.clear();
			t.computeStatistics(stats);
			this->assertEqual(t.getLoadFactor(), stats.getLoadFactor(), "Load factors must be equal (2).");
		}

		virtual void simpleRehash(void) {
			TableType t;
			t.rehash();
			t.insert(3);
			t.rehash();
			t.insert(4);
			t.rehash();
		}

		virtual void testCopyCtor(void) {
			TableType t;
			this->fillData(t);
			TableType b;
			b = t;
		}

		virtual void testMassiveFill(void) {
			TableType t;
			for (int i = 0; i < 5000; ++i) {
				t.insert(5 * i);
			}

			this->assertEqual(5000, t.getSize(), "After inserting n distinct elements we expect table length of n.");
		}

	};

} }

#endif /* TABLE_TEST_H */
