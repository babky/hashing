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
	class UniversalFunctionCWLF : public Hash::Systems::UniversalFunction<T, Storage> {
	public:
		explicit UniversalFunctionCWLF(size_t aTableSize = StorageParams::INITIAL_STORAGE_SIZE, size_t aUniversumMax = Hash::Math::Prime<T>::GREATEST_PRIME):
		  universumMax(aUniversumMax),
		  tableSize(aTableSize) {
			reset();
		}

		void setUniversumMax(T aUniversumMax) {
			universumMax = aUniversumMax;
		}

		T getUniversumMax(void) const {
			return universumMax;
		}

		void setTableSize(size_t size) {
			this->tableSize = size;
		}

		size_t getTableSize(void) const {
			return this->tableSize;
		}

		void reset(void) {
			Hash::Utils::IntegralGeneratorWrapper<T> g = Hash::Utils::IntegralGeneratorWrapper<T>(0, this->getUniversumMax());

			a = g.generate();
			b = g.generate();
		}

		size_t hash(const T & x) {
			return Hash::Math::UnsignedDoubleWord<T>::linear(a, x, b, universumMax) % tableSize;
		}

		size_t operator()(const T & a) {
			return hash(a);
		}

		void swap(UniversalFunctionCWLF & function) {
			std::swap(universumMax, function.universumMax);
			std::swap(a, function.a);
			std::swap(b, function.b);
			std::swap(tableSize, function.tableSize);
		}

	private:
		T universumMax;
		T a, b;
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
