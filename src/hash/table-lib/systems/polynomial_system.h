#ifndef POLYNOMIAL_SYSTEM_H
#define POLYNOMIAL_SYSTEM_H

#include "universal_system.h"
#include "math/double_word.h"
#include "storage.h"

namespace Hash { namespace Systems {

	template<typename T, class Storage>
	class PolynomialSystem : public UniversalFunction<T, Storage> {
	public:
		const static size_t START_DEGREE = 2;

		explicit PolynomialSystem(size_t aTableSize = StorageParams::INITIAL_STORAGE_SIZE, T aUniversumMax = Hash::Math::Prime<T>::GREATEST_PRIME, size_t aDegree = START_DEGREE):
		  universumMax(aUniversumMax),
		  coefficients(0),
		  tableSize(aTableSize),
		  degree(aDegree + 1) {
			this->reset();
		}
	
		PolynomialSystem(const PolynomialSystem & system):
		  universumMax(system.universumMax),
		  coefficients(0),
		  tableSize(system.tableSize),
		  degree(system.degree) {
			coefficients = new T[tableSize];

			for (size_t i = 0; i < degree; ++i) {
				coefficients[i] = system.coefficients[i];
			}
		}

		virtual ~PolynomialSystem(void) {
			delete [] coefficients;
			coefficients = 0;
		}

		PolynomialSystem & operator=(const PolynomialSystem & system) {
			PolynomialSystem tmp = system;
			tmp.swap(*this);
			return *this;
		}

		void setUniversumMax(T universumMax) {
			this->universumMax = universumMax;
		}

		inline T getUniversumMax(void) const {
			return this->universumMax;
		}

		void setTableSize(size_t size) {
			this->tableSize = size;
		}

		size_t getTableSize(void) const {
			return this->tableSize;
		}

		void reset(void) {
			Hash::Utils::IntegralGeneratorWrapper<T> g = Hash::Utils::IntegralGeneratorWrapper<T>(0, this->getUniversumMax());

			delete [] coefficients;
			coefficients = new T[degree];
			for (size_t i = 0; i < degree; ++i) {
				coefficients[i] = g.generate();
			}
		}

		size_t hash(const T & x) {
			using namespace Hash::Math;

			simple_assert(x < this->getUniversumMax(), "Given element is greater than the universum max.");

			T r = 0;
			for (size_t i = 0; i < degree; ++i) {
				r = UnsignedDoubleWord<T>::add(
						UnsignedDoubleWord<T>::multiply(r, x, universumMax), 
						coefficients[i], 
						universumMax
					);
			}

			return r % tableSize;
		}

		size_t operator()(const T & x) {
			return hash(x);
		}

		void swap(PolynomialSystem & system) {
			std::swap(universumMax, system.universumMax);
			std::swap(coefficients, system.coefficients);
			std::swap(degree, system.degree);
			std::swap(tableSize, system.tableSize);
		}

	private:
		T universumMax;
		T * coefficients;
		size_t tableSize;
		size_t degree;
	};

	template<typename T, class Storage>
	class PolynomialSystem2 : public Hash::Systems::PolynomialSystem<T, Storage> {
	public:
		explicit PolynomialSystem2(size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE,
			T aUniversumMax = Hash::Math::Prime<T>::GREATEST_PRIME, size_t aDegree = 2):
		  PolynomialSystem<T, Storage>(tableSize, aUniversumMax, aDegree) {
		}
	};

	template<typename T, class Storage>
	class PolynomialSystem4 : public Hash::Systems::PolynomialSystem<T, Storage> {
	public:
		explicit PolynomialSystem4(size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE,
			T aUniversumMax = Hash::Math::Prime<T>::GREATEST_PRIME, size_t aDegree = 4):
		  PolynomialSystem<T, Storage>(tableSize, aUniversumMax, aDegree) {
		}
	};

	template<typename T, class Storage>
	class PolynomialSystem5 : public Hash::Systems::PolynomialSystem<T, Storage> {
	public:
		explicit PolynomialSystem5(size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE,
			T aUniversumMax = Hash::Math::Prime<T>::GREATEST_PRIME, size_t aDegree = 5):
		  PolynomialSystem<T, Storage>(tableSize, aUniversumMax, aDegree) {
		}
	};

	template<typename T, class Storage>
	class PolynomialSystem32 : public Hash::Systems::PolynomialSystem<T, Storage> {
	public:
		explicit PolynomialSystem32(size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE,
			T aUniversumMax = Hash::Math::Prime<T>::GREATEST_PRIME, size_t aDegree = 32):
		  PolynomialSystem<T, Storage>(tableSize, aUniversumMax, aDegree) {
		}
	};

} }

namespace std {

	template <typename T, class Storage>
	void swap(Hash::Systems::PolynomialSystem<T, Storage> & a, Hash::Systems::PolynomialSystem<T, Storage> & b) {
		a.swap(b);
	}

}

#endif /* POLYNOMIAL_SYSTEM_H */
