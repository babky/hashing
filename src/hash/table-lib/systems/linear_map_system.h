#ifndef LINEAR_MAP_SYSTEM_H
#define LINEAR_MAP_SYSTEM_H

#include "systems/universal_system.h"
#include "utils/boost_include.h"
#include <utils/hash_assert.h>
#include <utils/static_random_generator.h>
#include <utils/hash_math.h>
#include <stdexcept>

namespace Hash { namespace Systems {

	/**
	 * Linear maps.
	 */
	template <typename T, class Storage>
	class UniversalFunctionLinearMap : public Hash::Systems::UniversalFunction<T, Storage> {
	public:
		typedef typename boost::uint_t<8 * sizeof(T)>::least UT;
		const static T UNIVERSUM_MAX_VALUE = boost::integer_traits<T>::const_max;
		const static size_t DEFAULT_TABLE_BIT_SIZE = 10;

		explicit UniversalFunctionLinearMap(size_t aTableBitSize = DEFAULT_TABLE_BIT_SIZE):
			tableBitSize(aTableBitSize),
			tableSize(1 << aTableBitSize),
			matrix(0) {
			reset();
		}

		~UniversalFunctionLinearMap(void) {
			delete [] this->matrix;
			this->matrix = 0;
		}

		size_t getTableSize(void) const {
			return this->tableSize;
		}

		void setTableSize(size_t size) {
			this->tableSize = size;
			this->tableBitSize = Hash::Math::log2exact(size);
			reset();
		}

		size_t getTableBitSize(void) const {
			return this->tableBitSize;
		}

		void reset(void) {
			delete [] this->matrix;
			this->matrix = new UT[this->tableBitSize];

			Hash::Utils::RandomGenerator<T> & g = Hash::Utils::StaticRandomGenerator<T>::getGenerator();
			for (size_t i = 0; i != this->tableBitSize; ++i) {
				this->matrix[i] = g.generate();
			}
		}

		size_t hash(const T & x) {
			// Represent the number in the form of a binary vector.
			size_t y = 0, digits, digits2;
			const size_t bits = boost::integer_traits<UT>::digits;

			// Parity accumulator of each column.
			UT c;

			// Interpret the value in the form without a sign.
			const UT & ux = reinterpret_cast<const UT &> (x);

			// Perform matrix and vector multiplication.
			for (size_t i = 0; i != this->tableBitSize; ++i) {
				c = this->matrix[i] & ux;

				// Find the parity of c.
				for (digits = bits / 2; digits != 0; digits /= 2) {
					digits2 = bits - digits;
					c = ((c << digits2) >> digits2) ^ (c >> digits);
				}

				simple_assert(c == 1 || c == 0, "Parity must be a bit.");

				y <<= 1;
				y |= (size_t) c;
			}

			simple_assert(y < tableSize, "Hash value must be in the range of the table.");

			return y;
		}

		size_t operator()(const T & a) {
			return hash(a);
		}

		T getUniversumMax(void) const {
			return UNIVERSUM_MAX_VALUE;
		}

		void setUniversumMax(T) {
			// TODO: Implement it.
			throw new std::invalid_argument("Can not change the universum max for the linear maps system.");
		}

		UniversalFunctionLinearMap(const UniversalFunctionLinearMap & rhs):
			tableSize(rhs.tableSize),
			tableBitSize(rhs.tableBitSize) {

			this->matrix = new UT[this->tableBitSize];
			for (size_t i = 0; i < this->tableBitSize; ++i) {
				this->matrix[i] = rhs.matrix[i];
			}
		}

		UniversalFunctionLinearMap & operator =(const UniversalFunctionLinearMap & rhs) {
			UniversalFunctionLinearMap cpy = rhs;
			swap(cpy);
			return *this;
		}
		
		void swap(UniversalFunctionLinearMap & b) {
			std::swap(tableSize, b.tableSize);
			std::swap(tableBitSize, b.tableBitSize);
			std::swap(matrix, b.matrix);
		}

		friend void swap(UniversalFunctionLinearMap & a, UniversalFunctionLinearMap & b) {
			a.swap(b);
		}

		class Generator {
		public:
			explicit Generator(T aUniversumMax, size_t tableSize):
				universumMax(aUniversumMax),
				hasNextFunction(true),
				f(Hash::Math::log2exact(tableSize)) {
				for (size_t i = 0; i != f.tableBitSize; ++i) {
					f.matrix[i] = universumMax;
				}
			}

			bool hasNext(void) const {
				return hasNextFunction;
			}

			UniversalFunctionLinearMap<T, Storage> next() {
				size_t i = 0;
				for (; i != f.tableBitSize; ++i) {
					if (f.matrix[i] == universumMax) {
						f.matrix[i] = 0;
					} else {
						++f.matrix[i];
						break;
					}
				}

				for (i = 0; i != f.tableBitSize; ++i) {
					if (f.matrix[i] < universumMax) {
						break;
					}
				}

				if (i == f.tableBitSize) {
					hasNextFunction = false;
				}

				return f;
			}

		private:
			UniversalFunctionLinearMap<T, Storage> f;
			T universumMax;
			bool hasNextFunction;
		};

	private:
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
		UT * matrix;
	};

} }

#endif /* LINEAR_MAP_SYSTEM_H */
