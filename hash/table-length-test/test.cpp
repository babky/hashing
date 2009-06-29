#include "test.h"
#include "utils/storage_statistics.h"
#include <iostream>

using namespace Hash;
using namespace Hash::Tests;
using namespace Hash::Utils;
using namespace std;

void LengthTest::runTest(void) {
	size_t maxChainLength = 0;

	const size_t TEST_COUNT = 384;
	const size_t TEST_LENGTH = 32767;

	for (int t = 0; t < TEST_COUNT; ++t) {
		TableCWLF table;
		for (int i = 0; i < TEST_LENGTH; ++i) {
			table.insert(i);
		}

		StorageStatistics stats;
		table.computeStatistics(stats);

		cout << stats << endl;

		for (int i = 0; i < TEST_LENGTH; ++i) {
			if (!table.contains(i)) {
				cout << "Error.\n";
			}
		}

		maxChainLength += stats.getMaximalChainLength();
	}

	cout << static_cast<double>(maxChainLength) / TEST_COUNT << "\n";
}

int main(size_t, const char **) {
	LengthTest t;
	t.runTest();

	return 0;
}
