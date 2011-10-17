#ifndef TABULATION_SYSTEM_H
#define TABULATION_SYSTME_H

#include <boost/config.hpp>
#ifdef BOOST_MSVC
	#pragma warning(disable: 4512 4127 4100)
#endif
#include <boost/integer.hpp>
#include <boost/integer_traits.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512 4127 4100)
#endif
#include "systems/universal_system.h"
#include <algorithm>

namespace Hash { namespace Systems {

	template<typename T>
	class TabulationFunction : public UniversalSystem<T> {
	public:
		static const size_t START_LENGTH = 16;

		explicit TabulationFunction(size_t length = START_LENGTH, size_t c = 8):
		  characterTableSize(1 << c),
		  fullTableSize(characterTableSize * DIGIT_NUMBER / c),
		  hashTableSize(length),
		  characterNumber(DIGIT_NUMBER / c),
		  characterDigits(c),
		  table(new size_t[characterTableSize * DIGIT_NUMBER / c])
		{
			setTableSize(length);
			reset();
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
		  table(new size_t[r.fullTableSize])
		{
			for (size_t i = 0; i < fullTableSize; ++i) {
				table[i] = r.table[i];
			}
		}

		TabulationFunction & operator =(const TabulationFunction & r) {
			TabulationFunction<T> tmp = r;
			tmp.swap(*this);
			return * this;
		}

		void reset(void) {
			for (size_t i = 0; i < fullTableSize; ++i) {
				table[i] = Hash::Utils::StaticRandomGenerator<size_t>::getGenerator().generate();
			}
		}

		size_t getTableSize(void) const {
			return hashTableSize;
		}
		
		void setTableSize(size_t size) {
			hashTableSize = size;
			outputMask = 0;

			for (size_t i = 1; i < size; i <<= 1) {
				outputMask |= i;
			}
		}

		T getUniversumMax(void) const {
			return boost::integer_traits<T>::const_max;
		}

		void setUniversumMax(T universumMax) {
		}

		size_t hash(const T & x, size_t length) {
			simple_assert(length == this->hashTableSize, "Lengths must be the same.");

			size_t result = 0;
			size_t mask = 0;

			for (size_t i = 0; i < characterDigits; ++i) {
				mask |= 1 << i;
			}

			for (size_t i = 0; i < characterNumber; ++i) {
				result ^= table[i * characterTableSize + (((mask << (i * characterDigits)) & x) >> i * characterDigits)];
			}

			return result & outputMask;
		}

		size_t operator()(const T & a, size_t length) {
			return hash(a, length);
		}

		void swap(TabulationFunction & r) {
			using std::swap;

			swap(characterTableSize, r.characterTableSize);
			swap(fullTableSize, r.fullTableSize);
			swap(hashTableSize, r.hashTableSize);
			swap(characterNumber, r.characterNumber);
			swap(characterDigits, r.characterDigits);
			swap(table, r.table);
		}

	private:
		/**
		 * The number of bits per key.
		 */
		const static size_t DIGIT_NUMBER = boost::integer_traits<T>::digits;

		size_t characterTableSize;
		size_t fullTableSize;
		size_t characterNumber;
		size_t characterDigits;
		size_t hashTableSize;
		size_t outputMask;
		size_t * table;
	};

} }

#endif /* TABULATION_SYSTEM_H */
