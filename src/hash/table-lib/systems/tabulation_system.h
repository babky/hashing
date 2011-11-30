#ifndef TABULATION_SYSTEM_H
#define TABULATION_SYSTEM_H

#include "../utils/boost_include.h"
#include "systems/universal_system.h"
#include "storage.h"
#include "../utils/static_random_generator.h"
#include <algorithm>

namespace Hash { namespace Systems {

	// TODO: Doc.
	template<typename T, class Storage, size_t CharBits>
	class TabulationFuncGen: public UniversalFunction<T, Storage> {
	public:
		static const size_t // computed compile-time parameters (template)
			CharTableSize = 1<<CharBits,
			CharCount = (boost::integer_traits<T>::digits-1) / CharBits + 1;
				//^ ceiling of x/y by the (x-1)/y +1 trick
	public:
		explicit TabulationFuncGen(size_t length = Hash::StorageParams::INITIAL_STORAGE_SIZE)
		{
			setTableSize(length);
		}

		// implicit destructor, copy constructor and assignment

		void reset(void) {
			for (size_t iChar = 0; iChar < CharCount; ++iChar) {
				for (size_t valChar = 0; valChar < CharTableSize; ++valChar) {
					table[iChar][valChar] = Hash::Utils::StaticRandomGenerator<T>::getGenerator().generate();
				}
			}
		}

		inline size_t getTableSize(void) const {
			return hashTableSize;
		}
		
		void setTableSize(size_t size) {
			hashTableSize = size;
			outputMask = size-1;
			simple_assert((size^outputMask)==0, "Size needs to be a power of two");
		}

		T getUniversumMax(void) const {
			return boost::integer_traits<T>::const_max;
		}

		void setUniversumMax(T) {
		}

		inline size_t hash(const T & x) {
			size_t result = 0;
			size_t mask = (1 << CharBits) - 1;

			for (size_t i = 0; i < CharCount; ++i) {
				size_t digit = (x >> (i*CharBits)) & mask;
				result ^= table[i][digit];
			}

			return result & outputMask;
		}

		inline size_t operator()(const T & a) {
			return hash(a);
		}

		void swap(TabulationFuncGen & r) {
			std::swap(*this, r);
		}

	protected:
		T table[CharCount][CharTableSize];
		size_t hashTableSize;
		size_t outputMask;
	};

	/**	Using 11 bits per character; for tests needing templates with two parameters */
	template<typename T, class Storage>
	class TabulationFunction: public TabulationFuncGen<T, Storage, 11> {
	};

} }

#endif /* TABULATION_SYSTEM_H */
