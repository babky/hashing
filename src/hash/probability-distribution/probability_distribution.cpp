#include <iostream>
#include "storages/collision_count_storage.h"
#include "systems/cwlf_system.h"
#include "systems/multiply_shift_system.h"
#include "systems/linear_map_system.h"
#include "utils/equality_comparer.h"

/**
 * Computes the PDF for linear functions
 *  - cwlf
 *  - multiply-shift
 *  - matrix multiplication (linear function/transformation)
 */

using namespace Hash;
using namespace Hash::Storages;
using namespace Hash::Systems;
using namespace Hash::Utils;

typedef std::vector<size_t> ElementVector;
typedef size_t T;

template<class Function>
bool collide(Function & f, const ElementVector & elements) {
	ElementVector::const_iterator b = elements.begin();
	size_t y = f.hash(*b);

	for (++b; b != elements.end(); ++b) {
		if (y != f.hash(*b)) {
			return false;
		}
	}

	return true;
}

template<class Function>
double collision_probability(const ElementVector & elements, size_t tableSize, size_t runs) {
	size_t collisions = 0;
	Function f;
	f.setTableSize(tableSize);

	for (size_t i = 0; i < runs; ++i) {
		f.reset();
		collisions += collide(f, elements) ? 1 : 0;
	}

	return static_cast<double>(collisions) / runs;

}

int main(int argc, char ** argv) {
	typedef UniversalFunctionCWLF<T, CollisionCountStorage<T, EqualityComparer<T>, size_t> > CWLFFunction;
	typedef MultiplyShiftSystem<T, CollisionCountStorage<T, EqualityComparer<T>, size_t> > MultiplyShiftFunction;
	typedef UniversalFunctionLinearMap<T, CollisionCountStorage<T, EqualityComparer<T>, size_t> > LinearMapFunction;

	size_t tableSize = 256;
	size_t universumMax = 1 << 24;
	size_t runs = universumMax / 1024;

	ElementVector v;
	for (size_t i = 2; i < universumMax; ++i) {
		v.clear();
		v.push_back(0);
		v.push_back(1);
		v.push_back(i);

		double cwlfProb = collision_probability<CWLFFunction>(v, tableSize, runs);
		double msProb = collision_probability<MultiplyShiftFunction>(v, tableSize, runs);
		double lmProb = collision_probability<LinearMapFunction>(v, tableSize, runs);

		std::cout << cwlfProb << "," << msProb << "," << lmProb << "\n";
	}


	return 0;
}
