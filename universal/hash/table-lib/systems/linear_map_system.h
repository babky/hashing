#ifndef LINEAR_MAP_SYSTEM_H
#define LINEAR_MAP_SYSTEM_H

#include "systems/universal_system.h"
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
#include <stdexcept>

namespace Hash {

	/**
	 * Linear maps.
	 */
	template <typename T>
	class UniversalFunctionLinearMap : public Hash::Systems::UniversalSystem<T> {
	public:
		const static T UNIVERSUM_MAX_VALUE = boost::integer_traits<T>::const_max;
		const static size_t START_LENGTH = 10;

		UniversalFunctionLinearMap():
		  tableBitSize(START_LENGTH),
		  tableSize(1 << START_LENGTH),
		  matrix(0) {
			reset();
		}

		~UniversalFunctionLinearMap(void) {
			delete [] this->matrix;
			this->matrix = 0;
		}

		Hash::Utils::RehashObserver * getRehashObserver(void) {
			return new RehashObserver(this);
		}

		size_t getTableSize(void) const {
			return this->tableSize;
		}

		void setTableSize(size_t size) {
			this->tableSize = size;
			this->tableBitSize = Hash::Math::log2exact(size);
		}

		size_t getTableBitSize(void) const {
			return this->tableBitSize;
		}

		void reset(void) {
			delete [] this->matrix;
			this->matrix = new T[this->tableBitSize];

			Hash::Utils::RandomGenerator<T> & g = Hash::Utils::StaticRandomGenerator<T>::getGenerator();
			for (size_t i = 0; i != this->tableBitSize; ++i) {
				this->matrix[i] = g.generate();
			}
		}

		size_t hash(const T & x, size_t length) {
			simple_assert(
				length == this->tableSize, 
				"Expected table's size differs from the current one for linear map universal function."
			);

			// Represent the number in the form of a binary vector.
			size_t y = 0;

			// Perform matrix and vector multiplication.
			T c, digits, digits2;
			const size_t bits = boost::integer_traits<T>::digits;

			for (size_t i = 0; i != this->tableBitSize; ++i) {
				c = this->matrix[i] & x;

				// Find the parity of c.
				for (digits = bits / 2; digits != 0; digits /= 2) {
					digits2 = bits - digits;
					c = ((c << digits2) >> digits2) ^ (c >> digits);
				}

				simple_assert(c <= 1, "Parity must be a bit.");

				y <<= 1;
				y += (size_t) c;
			}

			return y;
		}

		size_t operator()(const T & a, size_t length) {
			return hash(a, length);
		}

		void initialize(Hash::StorageInfo & info) {
			this->setTableSize(info.getTableSize());
			this->reset();
		}

		T getUniversumMax(void) const {
			return UNIVERSUM_MAX_VALUE;
		}

		void setUniversumMax(T) {
			// TODO: Nicer exception. Can not change the universum max for the linear maps system.
			// TODO: Better implement.
			throw new std::exception();
		}

		UniversalFunctionLinearMap(const UniversalFunctionLinearMap<T> & rhs):
			tableSize(rhs.tableSize),
			tableBitSize(rhs.tableBitSize) {

			this->matrix = new T[this->tableBitSize];
			for (size_t i = 0; i < this->tableBitSize; ++i) {
				this->matrix[i] = rhs.matrix[i];
			}
		}

		UniversalFunctionLinearMap<T> & operator =(const UniversalFunctionLinearMap<T> & rhs) {
			UniversalFunctionLinearMap<T> cpy = rhs;
			swap(*this, cpy);
			return *this;
		}

		friend void swap(UniversalFunctionLinearMap<T> & a, UniversalFunctionLinearMap<T> & b) {
			std::swap(a.tableSize, b.tableSize);
			std::swap(a.tableBitSize, b.tableBitSize);
			std::swap(a.matrix, b.matrix);
		}

	private:
		class RehashObserver : public Hash::Utils::RehashObserver {
		public:

			RehashObserver(UniversalFunctionLinearMap<T> * function):
			  f(function) {
			}

			virtual void rehash(Hash::StorageInfo & info) {
				this->f->initialize(info);
			}

		private:
			UniversalFunctionLinearMap<T> * f;
		};

		/**
		 * Length of the table. Power of two.
		 */
		size_t tableSize;

		/**
		 * Number of bits of the table size. 
		 */
		size_t tableBitSize;

		/**
		 * Pointer to the table - linear map description. Implementation invariant: always a zero pointer or a valid 
		 * one.
		 */
		T * matrix;
	};

}

#endif /* LINEAR_MAP_SYSTEM_H */
