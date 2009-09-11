#include "test.h"
#include "utils/storage_statistics.h"
#include <iostream>

using namespace Hash;
using namespace Hash::Tests;
using namespace Hash::Utils;
using namespace std;

void LengthTest::runTest(void) {
	size_t maxChainLength = 0;

	const size_t TEST_COUNT = 32;
	const size_t TEST_LENGTH = 1 << 24;

	RandomGenerator<size_t> generator(0, 8 * TEST_LENGTH, true);
	cout << "Running " << TEST_COUNT << " tests.\n";

	for (size_t t = 0; t < TEST_COUNT; ++t) {
		SimpleTable table;
		for (size_t i = 0; i < TEST_LENGTH; ++i) {
			try {
				table.insert(generator.generate());
			} catch(...) {
			}
		}

		StorageStatistics stats;
		table.computeStatistics(stats);

		cout << stats << endl;

		/* for (int i = 0; i < TEST_LENGTH; ++i) {
			if (!table.contains(i)) {
				cout << "Error.\n";
			}
		} */

		maxChainLength += stats.getMaximalChainLength();
	}

	cout << static_cast<double>(maxChainLength) / TEST_COUNT << "\n";
}

int main(size_t, const char **) {
	LengthTest t;
	t.runTest();

	return 0;
}
