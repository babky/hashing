#ifndef POLYNOMIAL_SYSTEM_H
#define POLYNOMIAL_SYSTEM_H

#include "universal_system.h"
#include "math/double_word.h"

namespace Hash { namespace Systems {

	template<typename T>
	class PolynomialSystem : public UniversalSystem<T> {
	public:
		const static size_t START_LENGTH = 16;
		const static size_t START_DEGREE = 2;

		explicit PolynomialSystem(size_t aLength = START_LENGTH, T aUniversumMax = Hash::Math::Prime<T>::GREATEST_PRIME, size_t aDegree = START_DEGREE):
		  universumMax(aUniversumMax),
		  coefficients(0),
		  length(aLength),
		  degree(aDegree + 1) {
			this->reset();
		}
	
		PolynomialSystem(const PolynomialSystem<T> & system):
		  universumMax(system.universumMax),
		  coefficients(0),
		  length(system.length),
		  degree(system.degree) {
			coefficients = new T[length];

			for (size_t i = 0; i < degree; ++i) {
				coefficients[i] = system.coefficients[i];
			}
		}

		virtual ~PolynomialSystem(void) {
			delete [] coefficients;
			coefficients = 0;
		}

		PolynomialSystem & operator=(const PolynomialSystem<T> & system) {
			PolynomialSystem<T> tmp = system;
			tmp.swap(*this);
			return *this;
		}

		Hash::Utils::RehashObserver * getRehashObserver(void) {
			return new RehashObserver(this);
		}

		void setUniversumMax(T universumMax) {
			this->universumMax = universumMax;
		}

		inline T getUniversumMax(void) const {
			return this->universumMax;
		}

		void setTableSize(size_t size) {
			this->length = size;
		}

		size_t getTableSize(void) const {
			return this->length;
		}

		void reset(void) {
			Hash::Utils::IntegralGeneratorWrapper<T> g = Hash::Utils::IntegralGeneratorWrapper<T>(0, this->getUniversumMax());

			delete [] coefficients;
			coefficients = new T[degree];
			for (size_t i = 0; i < degree; ++i) {
				coefficients[i] = g.generate();
			}
		}

		size_t hash(const T & x, size_t length) {
			using namespace Hash::Math;

			simple_assert(this->length == length, "Polynomial system table's size differs from the wanted size.");
			simple_assert(x < this->getUniversumMax(), "Given element is greater than the universum max.");

			T r = 0;
			for (size_t i = 0; i < degree; ++i) {
				r = UnsignedDoubleWord<T>::add(
						UnsignedDoubleWord<T>::multiply(r, x, universumMax), 
						coefficients[i], 
						universumMax
					);
			}

			return r % length;
		}

		size_t operator()(const T & x, size_t length) {
			return hash(x, length);
		}

		void initialize(Hash::StorageInfo & info) {
			this->setTableSize(info.getTableSize());
			this->reset();
		}

		void swap(PolynomialSystem<T> & system) {
			std::swap(universumMax, system.universumMax);
			std::swap(coefficients, system.coefficients);
			std::swap(degree, system.degree);
			std::swap(length, system.length);
		}

	private:
		class RehashObserver : public Hash::Utils::RehashObserver {
		public:
			RehashObserver(PolynomialSystem<T> * function):
			  f(function){
			}

			void rehash(Hash::StorageInfo & info) {
				this->f->initialize(info);
			}
				
		private:
			PolynomialSystem<T> * f;
		};

		T universumMax;
		T * coefficients;
		size_t length;
		size_t degree;
	};

} }

namespace std {

	template <typename T>
	void swap(Hash::Systems::PolynomialSystem<T> & a, Hash::Systems::PolynomialSystem<T> & b) {
		a.swap(b);
	}

}

#endif /* POLYNOMIAL_SYSTEM_H */
