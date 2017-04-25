#ifndef RANDOM_SYSTEM_H
#define RANDOM_SYSTEM_H

#include <algorithm>
#include "systems/universal_system.h"
#include "utils/hash_assert.h"
#include "math/double_word.h"
#include "storage.h"
#include "utils/static_random_generator.h"

namespace Hash { namespace Systems {

	template <typename T, class Storage>
	class RandomBin : public Hash::Systems::UniversalFunction<T, Storage> {
	public:
		virtual ~RandomBin(void) {
		}

		explicit RandomBin(size_t aTableSize = StorageParams::INITIAL_STORAGE_SIZE):
		  tableSize(aTableSize),
		  g(Hash::Utils::IntegralGeneratorWrapper<T>(0, aTableSize)) {
			reset();
		}

		void setTableSize(size_t size) {
			this->tableSize = size;
		}

		size_t getTableSize(void) const {
			return this->tableSize;
		}

		void reset(void) {
			g = Hash::Utils::IntegralGeneratorWrapper<T>(0, getTableSize());
		}

		size_t hash(const T & x) {
			return g.generate();
		}

		virtual T getUniversumMax(void) const {
			return boost::integer_traits<T>::const_max;
		}

		virtual void setUniversumMax(T) {
		}

		size_t operator()(const T & a) {
			return hash(a);
		}

		void swap(RandomBin & function) {
			std::swap(g, function.g);
			std::swap(tableSize, function.tableSize);
		}

	private:
		size_t tableSize;
		Hash::Utils::IntegralGeneratorWrapper<T> g;
	};

} }

namespace std {

	template <typename T, class Storage>
	void swap(Hash::Systems::RandomBin<T, Storage> & a, Hash::Systems::RandomBin<T, Storage> & b) {
		a.swap(b);
	}

}

#endif /* RANDOM_SYSTEM */
