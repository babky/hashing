#ifndef MULTIPLY_SHIFT_SYSTEM_H
#define MULTIPLY_SHIFT_SYSTEM_H

#include <algorithm>
#include "systems/universal_system.h"
#include "utils/hash_assert.h"
#include "math/double_word.h"
#include "utils/hash_math.h"
#include "storage.h"
#include "utils/static_random_generator.h"

namespace Hash { namespace Systems {

	/**
	 * Dietzfelbinger's multiply shift system.
	 */
	template <typename T, class Storage>
	class MultiplyShiftSystem : public Hash::Systems::UniversalFunction<T, Storage> {
	public:
		explicit MultiplyShiftSystem(size_t aTableSize = StorageParams::INITIAL_STORAGE_SIZE, size_t aUniversumMax = 0):
		  tableSize(aTableSize) {
			setUniversumMax(aUniversumMax);
			reset();
		}

		void setUniversumMax(T aUniversumMax) {
			if (aUniversumMax == 0) {
				universumMax = std::numeric_limits<T>::max();
			} else {
				universumMax = aUniversumMax;
			}

			// Remove the bits overflowing the universum max.
			lshift = sizeof(T) * 8 - Hash::Math::log2ceil(universumMax);
		}

		T getUniversumMax(void) const {
			return universumMax;
		}

		void setTableSize(size_t size) {
			this->tableSize = size;
			// Shift the bits so that the log2(size) bits are the rightmost ones.
			this->rshift =  sizeof(T) * 8 - Hash::Math::log2exact(size);
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
			size_t hv = ((a * x) << lshift) >> rshift;
			return hv;
		}

		size_t operator()(const T & x) {
			return hash(x);
		}

		void swap(MultiplyShiftSystem & function) {
			std::swap(universumMax, function.universumMax);
			std::swap(a, function.a);
			std::swap(lshift, function.lshift);
			std::swap(rshift, function.rshift);
			std::swap(tableSize, function.tableSize);
		}

		class Generator {
		public:
			explicit Generator(T aUniversumMax, size_t tableSize) {
				f.setUniversumMax(aUniversumMax);
				f.setTableSize(tableSize);
				f.a = 0;
			}

			bool hasNext(void) const {
				return f.a < f.universumMax;
			}

			MultiplyShiftSystem<T, Storage> next() {
				// First call.
				if (f.a == 0) {
					f.a = 1;
					return f;
				}

				f.a += 2;
				return f;
			}

		private:
			MultiplyShiftSystem<T, Storage> f;
		};

	private:
		T universumMax;
		T a;
		size_t lshift, rshift;
		size_t tableSize;
	};

} }

namespace std {

	template <typename T, class Storage>
	void swap(Hash::Systems::MultiplyShiftSystem<T, Storage> & a, Hash::Systems::MultiplyShiftSystem<T, Storage> & b) {
		a.swap(b);
	}

}

#endif /* MULTIPLY_SHIFT_SYSTEM_H */
