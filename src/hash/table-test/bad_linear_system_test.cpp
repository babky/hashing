#include "systems/bad_linear_system.h"
#include "storages/chained_storage.h"
#include "utils/equality_comparer.h"
#include "test.h"
#include <iostream>

using namespace Hash::Storages;
using namespace Hash::Systems;
using namespace Hash::Utils;
using namespace TestSuite;
using namespace std;

typedef ChainedStorage<size_t, EqualityComparer<size_t>, size_t> Storage;

class BadLinearSystemForTest : public BadLinearSystem<size_t, Storage> {
public:
	explicit BadLinearSystemForTest(size_t aP):
		BadLinearSystem<size_t, Storage>(aP) {
	}

	void setFunction(size_t a, size_t b) {
		A = a;
		B = b;
	}
};

int main(int argc, char ** argv) {
	const size_t p = 17;
	BadLinearSystemForTest bls(p);
	Assert assert;

	for (size_t a = 0; a < p; ++a) {
		for (size_t b = 0; b < p; ++b) {
			bls.setFunction(a, b);

			for (size_t x = 0; x < p; ++x) {
				for (size_t y = 0; y < p; ++ y) {
					size_t r = bls.hash(x * p + y);
					assert.assertEqual(r, (x * b + y * a) % p, "The hash value must be correct.");
				}
			}
		}
	}

	cout << "BadLinearSystemTest results" << endl;
	cout << assert << endl;

	return 0;
}
