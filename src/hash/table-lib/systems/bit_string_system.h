#ifndef BIT_STRING_SYSTEM_H
#define BIT_STRING_SYSTEM_H

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
#include "utils/static_random_generator.h"

namespace Hash { namespace Systems {

	/**
	 * The simplified bit string system. It is a universal system with the constant 1.
	 */
	template<typename T>
	class BitStringFunction : public UniversalSystem<T> {
	public:

		/**
		 * Initial table size.
		 */
		static const size_t START_LENGTH = 16;

		~BitStringFunction(void) {
			delete [] coefficients;
		}

		explicit BitStringFunction(size_t length = START_LENGTH):
		  coefficients(new size_t[COEFFICIENT_NUMBER])
		{
			setTableSize(length);
			reset();
		}

		BitStringFunction(const BitStringFunction & r):
		  coefficients(new size_t[COEFFICIENT_NUMBER]),
		  tableSize(r.tableSize),
		  mask(r.mask)
		{
			for (size_t i = 0; i < COEFFICIENT_NUMBER; ++i) {
				coefficients[i] = r.coefficients[i];
			}
		}

		BitStringFunction & operator =(const BitStringFunction & r) {
			BitStringFunction tmp = r;
			tmp.swap(*this);
			return *this;
		}

		void reset(void) {
			for (size_t i = 0; i < COEFFICIENT_NUMBER; ++i) {
				coefficients[i] = Hash::Utils::StaticRandomGenerator<size_t>::getGenerator().generate();
			}
		}

		size_t getTableSize(void) const {
			return tableSize;
		}
		
		void setTableSize(size_t size) {
			tableSize = size;
			
			mask = 0;
			for (size_t i = 1; size > i; i <<= 1) {
				mask |= i;
			}
		}

		T getUniversumMax(void) const {
			return boost::integer_traits<T>::const_max;
		}

		void setUniversumMax(T universumMax) {
		}

		size_t hash(const T & x, size_t length) {
			simple_assert(length == this->tableSize, "Lengths must be the same.");

			T coefficientMask = 1;
			size_t result = 0;

			for (size_t i = 0; i < COEFFICIENT_NUMBER; ++i) {
				result ^= (x & coefficientMask) * coefficients[i];
				coefficientMask <<= 1;
			}

			return result & mask;
		}

		size_t operator()(const T & a, size_t length) {
			return hash(a, length);
		}

		void swap(BitStringFunction & r) {
			using std::swap;

			swap(this->coefficients, r.coefficients);
			swap(this->tableSize, r.tableSize);
			swap(this->mask, r.mask);
		}

	private:
		/**
		 * The number of bits of the underlying type - the number of coefficients.
		 */
		static const size_t COEFFICIENT_NUMBER = boost::integer_traits<T>::digits;

		/**
		 * The array of coefficients.
		 */
		size_t * coefficients;

		/**
		 * Current table size.
		 */
		size_t tableSize;

		/**
		 * The mask - which bits apply to create a hash value.
		 */
		size_t mask;
	};

} }

#endif /* BIT_STRING_SYSTEM_H */
