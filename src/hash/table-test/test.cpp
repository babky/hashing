#include "table_test.h"
#include "storages/probing_storage.h"
#include "storages/direct_chaining_storage.h"
#include "systems/polynomial_system.h"
#include "systems/multiply_shift_system.h"
#include "systems/two_way_system_randomized.h"
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

	cout << "test_mult_shift" << endl;
	TableTest<Table<size_t, EqualityComparer<size_t>, MultiplyShiftSystem, ChainedStorage> > test_mult_shift;
	test_mult_shift.runTest();
	cout << test_mult_shift.getAssertionResult() << endl << endl;

	cout << "test_direct_chain" << endl;
	TableTest<Table<boost::uint_fast64_t, EqualityComparer<boost::uint_fast64_t>, PolynomialSystem, DirectChainingStorage> > test_direct_chain;
	test_direct_chain.runTest();
	cout << test_direct_chain.getAssertionResult() << endl << endl;

	cout << "test_univ_pol_32b" << endl;
	TableTest<Table<boost::uint_fast64_t, EqualityComparer<boost::uint_fast64_t>, PolynomialSystem, ChainedStorage> > test_univ_pol_32b;
	test_univ_pol_32b.runTest();
	cout << test_univ_pol_32b.getAssertionResult() << endl << endl;

	cout << "test_univ_pol_64b" << endl;
	TableTest<Table<boost::uint_fast64_t, EqualityComparer<boost::uint_fast64_t>, PolynomialSystem, ChainedStorage> > test_univ_pol_64b;
	test_univ_pol_64b.runTest();
	cout << test_univ_pol_64b.getAssertionResult() << endl << endl;

	cout << "test_univ_cwlf" << endl;
	TableTest<Table<size_t, EqualityComparer<size_t>, UniversalFunctionCWLF, ChainedStorage> > test_univ_cwlf;
	test_univ_cwlf.runTest();
	cout << test_univ_cwlf.getAssertionResult() << endl << endl;

	cout << "test_hash" << endl;
	TableTest<Table<int, EqualityComparer<int>, Function, ChainedStorage> > test_hash;
	test_hash.runTest();
	cout << test_hash.getAssertionResult() << endl << endl;

	cout << "test_univ_lin_map" << endl;
	TableTest<Table<int, EqualityComparer<int>, UniversalFunctionLinearMap, ChainedStorage> > test_univ_lin_map;
	test_univ_lin_map.runTest();
	cout << test_univ_lin_map.getAssertionResult() << endl << endl;

	cout << "test_univ_lin_map_bs" << endl;
	TableTest<Table<int, EqualityComparer<int>, UniversalFunctionLinearMap, BoundedChainedStorage, GuaranteedRehashPolicy> > test_univ_lin_map_bs;
	test_univ_lin_map_bs.runTest();
	cout << test_univ_lin_map_bs.getAssertionResult() << endl << endl;
	
	cout << "test_univ_lin_map_2wr_bs" << endl;
	TableTest<Table<int, EqualityComparer<int>, TwoWaySystemRandomizedLinearMap, BoundedChainedStorage, GuaranteedRehashPolicy> > test_univ_lin_map_2wr_bs;
	test_univ_lin_map_2wr_bs.runTest();
	cout << test_univ_lin_map_2wr_bs.getAssertionResult() << endl << endl;

	cout << "test_univ_lin_map_2w_bs" << endl;
	TableTest<Table<int, EqualityComparer<int>, TwoWaySystemLinearMap, BoundedChainedStorage, GuaranteedRehashPolicy> > test_univ_lin_map_2w_bs;
	test_univ_lin_map_2w_bs.runTest();
	cout << test_univ_lin_map_2w_bs.getAssertionResult() << endl << endl;

	cout << "test_lin_prob_cwlf" << endl;
	TableTest<Table<size_t, EqualityComparer<size_t>, UniversalFunctionCWLF, LinearProbingStorage> > test_lin_prob_cwlf;
	test_lin_prob_cwlf.runTest();
	cout << test_lin_prob_cwlf.getAssertionResult() << endl << endl;

	cout << "test_quad_prob_cwlf" << endl;
	TableTest<Table<size_t, EqualityComparer<size_t>, UniversalFunctionCWLF, QuadraticProbingStorage> > test_quad_prob_cwlf;
	test_quad_prob_cwlf.runTest();
	cout << test_quad_prob_cwlf.getAssertionResult() << endl << endl;

	cout << "mathTest" << endl;
	MathTest mathTest;
	mathTest.runTest();
	cout << mathTest.getAssertionResult() << endl << endl;

	return 0;
}
