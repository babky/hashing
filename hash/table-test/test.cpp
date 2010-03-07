#include "table_test.h"
#include <iostream>

int main(size_t, const char **) {
	using namespace Hash;
	using namespace Hash::Utils;
	using namespace Hash::Tests;
	using namespace Hash::Storages;
	using namespace Hash::Policies::Rehash;
	using namespace std;

	TableTest<Table<int, EqualityComparer<int>, UniversalFunctionCWLF, ChainedStorage> > test_univ_cwlf;
	test_univ_cwlf.runTest();
	cout << test_univ_cwlf.getAssertionResult();

	TableTest<Table<int, EqualityComparer<int>, Function, ChainedStorage> > test_hash;
	test_hash.runTest();
	cout << test_hash.getAssertionResult();

	TableTest<Table<int, EqualityComparer<int>, UniversalFunctionLinearMap, ChainedStorage> > test_univ_lin_map;
	test_univ_lin_map.runTest();
	cout << test_univ_lin_map.getAssertionResult();

	TableTest<Table<int, EqualityComparer<int>, UniversalFunctionLinearMap, BoundedChainedStorage, GuaranteedRehashPolicy> > test_univ_lin_map_bs;
	test_univ_lin_map_bs.runTest();
	cout << test_univ_lin_map_bs.getAssertionResult();

	return 0;
}
