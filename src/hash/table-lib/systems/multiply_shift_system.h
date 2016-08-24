#ifndef CWLF_SYSTEM_H
#define CWLF_SYSTEM_H

#include <algorithm>
#include "systems/universal_system.h"
#include "utils/hash_assert.h"
#include "math/double_word.h"
#include "storage.h"
#include "utils/static_random_generator.h"

namespace Hash { namespace Systems {

	/**
	 * Carter - Wegman system of linear functions.
	 */
	template <typename T, class Storage>
	class MultiplyShiftSystem : public Hash::Systems::UniversalFunction<T, Storage> {
	public:
		explicit MultiplyShiftSystem(size_t aTableSize = StorageParams::INITIAL_STORAGE_SIZE, size_t aUniversumMax = 0):
		  universumMax(aUniversumMax),
		  tableSize(aTableSize) {
			reset();
		}

		void setUniversumMax(T aUniversumMax) {
			universumMax = aUniversumMax;

			if (universumMax == 0) {
				universumMax = std::numeric_limits<T>::max;
			}

			lshift = sizeof(T) * 8 - Hash::Math::log2ceil(aUniversumMax);
		}

		T getUniversumMax(void) const {
			return universumMax;
		}

		void setTableSize(size_t size) {
			this->tableSize = size;
			this->rshift = lshift + Hash::Math::log2ceil(universumMax) - Hash::Math::log2exact(size);
		}

		size_t getTableSize(void) const {
			return this->tableSize;
		}

		void reset(void) {
			// We need an odd number in the range of universe.
			// The half of max + 1 is the valid range for generating the values which
			// are then mapped to odd values.
			Hash::Utils::IntegralGeneratorWrapper<T> g = Hash::Utils::IntegralGeneratorWrapper<T>(0, this->getUniversumMax() / 2 + 1);
			a = g.generate() * 2 + 1;
		}

		size_t hash(const T & x) {
			size_t hv = ((a * x) << rshift) >> lshift;
			return hv;
		}

		size_t operator()(const T & a) {
			return hash(a);
		}

		void swap(MultiplyShiftSystem & function) {
			std::swap(universumMax, function.universumMax);
			std::swap(a, function.a);
			std::swap(lshift, function.lshift);
			std::swap(rshift, function.rshift);
			std::swap(tableSize, function.tableSize);
		}

	private:
		T universumMax;
		T a;
		size_t lshift, rshift;
		size_t tableSize;
	};

} }

namespace std {

	template <typename T, class Storage>
	void swap(Hash::Systems::UniversalFunctionCWLF<T, Storage> & a, Hash::Systems::UniversalFunctionCWLF<T, Storage> & b) {
		a.swap(b);
	}

}

#endif /* CWLF_SYSTEM */
