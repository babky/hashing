#include "table_test.h"
#include "systems/polynomial_system.h"
#include "math/double_word.h"
#include <iostream>

class MathTest : public TestSuite::DefaultTest {
public:
	virtual void runTest(void) {
		using namespace Hash::Math;
		using namespace std;

		size_t r;
		for (size_t u = 0; u < 256; ++u) {
			for (size_t l = 0; l < 256; ++l) {
				for (size_t m = 1; m < 256; ++m) {
					r = UnsignedDoubleWord<unsigned char>::modulo((unsigned char) u, (unsigned char) l, (unsigned char) m);
					if (r != (u * 256 + l) % m) {
						cout << "(" << u << " * " << 256 << " + " << l << ") % " << m << "\n";
					}
					this->assertEqual<size_t>(r, (u * 256 + l) % m, "Modulo test.");

					r = UnsignedDoubleWord<unsigned char>::add((unsigned char) u, (unsigned char) l, (unsigned char) m);
					if (r != (u + l) % m) {
						cout << "(" << u << " + " << l << ") % " << m << "\n";
					}
					this->assertEqual<size_t>(r, (u + l) % m, "Add test.");

					r = UnsignedDoubleWord<unsigned char>::multiply((unsigned char) u, (unsigned char) l, (unsigned char) m);
					if (r != (u * l) % m) {
						cout << "(" << u << " * " << l << ") % " << m << "\n";
					}
					this->assertEqual<size_t>(r, (u * l) % m, "Mul test.");
				}
			}
		}
	}
};

int main(int, const char **) {
	using namespace Hash;
	using namespace Hash::Utils;
	using namespace Hash::Systems;
	using namespace Hash::Tests;
	using namespace Hash::Storages;
	using namespace Hash::Policies::Rehash;
	using namespace std;

	TableTest<Table<size_t, EqualityComparer<boost::uint_fast64_t>, PolynomialSystem, ChainedStorage> > test_univ_pol_32b;
	test_univ_pol_32b.runTest();
	cout << test_univ_pol_32b.getAssertionResult();

	TableTest<Table<boost::uint_fast64_t, EqualityComparer<boost::uint_fast64_t>, PolynomialSystem, ChainedStorage> > test_univ_pol_64b;
	test_univ_pol_64b.runTest();
	cout << test_univ_pol_64b.getAssertionResult();

	TableTest<Table<size_t, EqualityComparer<size_t>, UniversalFunctionCWLF, ChainedStorage> > test_univ_cwlf;
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

	/*TableTest<Table<int, EqualityComparer<int>, TwoWaySystemLinearMap, BoundedChainedStorage, GuaranteedRehashPolicy> > test_univ_lin_map_2w_bs;
	test_univ_lin_map_2w_bs.runTest();
	cout << test_univ_lin_map_2w_bs.getAssertionResult();*/

	MathTest mathTest;
	mathTest.runTest();
	cout << mathTest.getAssertionResult();

	return 0;
}
