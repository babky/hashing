#include "table_test.h"
#include <iostream>

int main(size_t, const char **) {
	using namespace Hash;
	using namespace Hash::Tests;
	using namespace std;

	TableTest test;
	test.runTest();
	cout << test.getAssertionsResult();

	return 0;
}
