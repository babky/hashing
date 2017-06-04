#ifndef TABULATION_SYSTEM_H
#define TABULATION_SYSTEM_H

#include "../utils/boost_include.h"
#include "systems/universal_system.h"
#include "storage.h"
#include "../utils/static_random_generator.h"
#include <algorithm>

namespace Hash { namespace Systems {

	template<typename T, class Storage>
	class TabulationFunction : public UniversalFunction<T, Storage> {
		/**
		 * Simple tabulation system as described in https://arxiv.org/abs/1011.5200.
		 */
	public:
		explicit TabulationFunction(size_t length = Hash::StorageParams::INITIAL_STORAGE_SIZE, size_t c = 8):
		  characterTableSize(1 << c),
		  characterNumber(DIGIT_NUMBER / c),
		  fullTableSize(characterTableSize * characterNumber),
		  hashTableSize(length),
		  characterDigits(c),
		  table(new size_t[characterTableSize * characterNumber])
		{
			setTableSize(length);
		}

		~TabulationFunction(void) {
			delete [] table;
		};

		TabulationFunction(const TabulationFunction & r):
		  characterTableSize(r.characterTableSize),
		  fullTableSize(r.fullTableSize),
		  hashTableSize(r.hashTableSize),
		  characterNumber(r.characterNumber),
		  characterDigits(r.characterDigits),
		  table(new size_t[r.fullTableSize]),
		  outputMask(r.outputMask)
		{
			for (size_t i = 0; i < fullTableSize; ++i) {
				table[i] = r.table[i];
			}
		}

		TabulationFunction & operator =(const TabulationFunction & r) {
			TabulationFunction tmp = r;
			tmp.swap(*this);
			return *this;
		}

		void reset(void) {
			for (size_t i = 0; i < fullTableSize; ++i) {
				table[i] = Hash::Utils::StaticRandomGenerator<size_t>::getGenerator().generate();
			}
		}

		inline size_t getTableSize(void) const {
			return hashTableSize;
		}
		
		void setTableSize(size_t size) {
			hashTableSize = size;

			outputMask = 0;
			for (size_t i = 1; size > i; i <<= 1) {
				outputMask |= i;
			}
		}

		T getUniversumMax(void) const {
			return boost::integer_traits<T>::const_max;
		}

		void setUniversumMax(T) {
		}

		inline size_t hash(const T & x) {
			size_t result = 0;
			size_t mask = (1 << characterDigits) - 1;

			for (size_t i = 0; i < characterNumber; ++i) {
				result ^= table[i * characterTableSize + (((mask << (i * characterDigits)) & x) >> i * characterDigits)];
			}

			return result & outputMask;
		}

		inline size_t operator()(const T & a) {
			return hash(a);
		}

		void swap(TabulationFunction & r) {
			using std::swap;

			swap(characterTableSize, r.characterTableSize);
			swap(fullTableSize, r.fullTableSize);
			swap(hashTableSize, r.hashTableSize);
			swap(characterNumber, r.characterNumber);
			swap(characterDigits, r.characterDigits);
			swap(outputMask, r.outputMask);
			swap(table, r.table);
		}

	private:
		/**
		 * The number of bits per key.
		 */
		const static size_t DIGIT_NUMBER = boost::integer_traits<T>::digits;

		size_t characterNumber;
		size_t characterTableSize;
		size_t fullTableSize;
		size_t characterDigits;
		size_t hashTableSize;
		size_t outputMask;
		size_t * table;
	};

} }

#endif /* TABULATION_SYSTEM_H */
