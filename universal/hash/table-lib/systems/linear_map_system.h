#ifndef LINEAR_MAP_SYSTEM_H
#define LINEAR_MAP_SYSTEM_H

#include <boost/config.hpp>
#ifdef BOOST_MSVC
	#pragma warning(disable: 4512 4127 4100)
#endif
#include <boost/integer_traits.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512 4127 4100)
#endif
#include <utils/hash_assert.h>
#include <utils/static_random_generator.h>
#include <utils/hash_math.h>
#include <utils/rehash_observer.h>

namespace Hash {

	/**
	 * Linear maps.
	 */
	template <typename Type>
	class UniversalFunctionLinearMap {
	public:
		const static size_t UNIVERSUM_MAX_VALUE = boost::integer_traits<size_t>::const_max;
		const static size_t START_LENGTH = 10;

		typedef Type HashType;

		UniversalFunctionLinearMap():
		  tableBitSize(START_LENGTH),
		  tableSize(1 << START_LENGTH),
		  T(0) {
			reset();
		}

		~UniversalFunctionLinearMap(void) {
			delete [] this->T;
			this->T = 0;
		}

		Hash::Utils::RehashObserver * getRehashObserver(void) {
			return new RehashObserver(this);
		}

		void setTableSize(size_t length) {
			this->tableSize = length;
			this->tableBitSize = Hash::Math::log2exact(length);
		}

		size_t getTableSize(void) const {
			return this->tableSize;
		}

		size_t getTableBitSize(void) const {
			return this->tableBitSize;
		}

		void reset(void) {
			delete [] this->T;
			this->T = new size_t[this->tableBitSize];

			Hash::Utils::RandomGenerator<size_t> & g = Hash::Utils::StaticRandomGenerator<size_t>::getGenerator();
			for (size_t i = 0; i != this->tableBitSize; ++i) {
				this->T[i] = g.generate();
			}
		}

		size_t hash(const HashType & x, size_t length) {
			simple_assert(
				length == this->tableSize, 
				"Expected table's size differs from the current one for linear map universal function."
			);

			// Represent a number in the form of a binary vector.
			size_t y = 0;

			// Perform matrix and vector multiplication.
			size_t c, digits, digits2;
			const size_t bits = boost::integer_traits<size_t>::digits;

			for (size_t i = 0; i != this->tableBitSize; ++i) {
				c = this->T[i] & x;
				for (digits = bits / 2; digits != 0; digits /= 2) {
					digits2 = bits - digits;
					c = ((c << digits2) >> digits2) ^ (c >> digits);
				}

				simple_assert(c <= 1, "Parity must be a bit.");

				y <<= 1;
				y += c;
			}

			return y;
		}

		size_t operator()(const HashType & a, size_t length) {
			return hash(a, length);
		}

		void initialize(Hash::StorageInfo & info) {
			this->setTableSize(info.getTableSize());
			this->reset();
		}

		UniversalFunctionLinearMap(const UniversalFunctionLinearMap<HashType> & rhs):
			tableSize(rhs.tableSize),
			tableBitSize(rhs.tableBitSize) {

			this->T = new size_t[this->tableBitSize];
			for (size_t i = 0; i < this->tableBitSize; ++i) {
				this->T[i] = rhs.T[i];
			}
		}

		UniversalFunctionLinearMap<HashType> & operator =(const UniversalFunctionLinearMap<HashType> & rhs) {
			UniversalFunctionLinearMap<HashType> cpy = rhs;
			swap(*this, cpy);
			return *this;
		}

		friend void swap(UniversalFunctionLinearMap<HashType> & a, UniversalFunctionLinearMap<HashType> & b) {
			std::swap(a.tableSize, b.tableSize);
			std::swap(a.tableBitSize, b.tableBitSize);
			std::swap(a.T, b.T);
		}

	private:
		class RehashObserver : public Hash::Utils::RehashObserver {
		public:

			RehashObserver(UniversalFunctionLinearMap<HashType> * function):
			  f(function) {
			}

			virtual void rehash(Hash::StorageInfo & info) {
				this->f->initialize(info);
			}

		private:
			UniversalFunctionLinearMap<HashType> * f;

		};

		/**
		 * Length of the table.
		 */
		size_t tableSize;

		/**
		 * Number of 
		 */
		size_t tableBitSize;

		/**
		 * Pointer to the table - linear map description. Implementation invariant: always a zero pointer or a valid 
		 * one.
		 */
		size_t * T;
	};

}

#endif /* LINEAR_MAP_SYSTEM_H */
