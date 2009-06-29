#include "test.h"
#include <iostream>

using namespace Hash;
using namespace Hash::Tests;
using namespace Hash::Utils;
using namespace std;

void LengthTest::runTest(void) {
	for (int t = 0; t < 500; ++t) {
		TableCWLF table;
		for (int i = 0; i < 50000; ++i) {
			table.insert(i);
		}

		StorageStatistics stats;
		table.computeStatistics(stats);

		// cout << stats << endl;
	}
}

int main(size_t, const char **) {
	LengthTest t;
	t.runTest();

	return 0;
}
