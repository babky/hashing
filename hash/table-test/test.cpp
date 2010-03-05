#include "table_test.h"
#include <iostream>

int main(size_t, const char **) {
	using namespace Hash;
	using namespace Hash::Utils;
	using namespace Hash::Tests;
	using namespace Hash::Storages;
	using namespace std;

	TableTest<Table<int, EqualityComparer<int>, UniversalFunctionCWLF, ChainedStorage> > test_univ_cwlf;
	test_univ_cwlf.runTest();
	cout << test_univ_cwlf.getAssertionsResult();

	TableTest<Table<int, EqualityComparer<int>, Function, ChainedStorage> > test_hash;
	test_hash.runTest();
	cout << test_hash.getAssertionsResult();

	return 0;
}
