#ifndef BAD_LINEAR_SYSTEM_H
#define BAD_LINEAR_SYSTEM_H

#include <stdexcept>
#include <string>
#include <sstream>
#include "universal_system.h"
#include "../generators/random_generator.h"
#include "../math/double_word.h"
#include "../utils/hash_math.h"

namespace Hash { namespace Systems {

	class BadLinearSystemTableSizeOperation : public std::exception {
	public:
		virtual ~BadLinearSystemTableSizeOperation(void) {
		}

		virtual const char * what() const noexcept {
			return "The size of the table can not be changed.";
		}
	};

	/**
	 * Bad linear system works uses fields over Z_p.
	 *
	 * The elements from universe are of the form (a, b), where a, b come from [p]
	 * The coefficients are again numbers A, B from [p].
	 * The result is A * a + B * b taken again modulo p.
	 */
	template<typename T, class Storage>
	class BadLinearSystem: public Hash::Systems::UniversalFunction<T, Storage> {
	public:
		explicit BadLinearSystem(size_t aP = 17):
			p(aP),
			g(0, aP-1) {
			reset();
		}

		/**
		 * D-tor.
		 */
		virtual ~BadLinearSystem(void) {
		};

		/**
		 * Resulting hash type.
		 */
		typedef size_t HashType;

		/**
		 * Resets the function - chooses another from the universal system.
		 */
		virtual void reset(void) {
			A = g.generate();
			B = g.generate();
		}

		/**
		 * Current size of the hash table.
		 *
		 * @return The size of the hash table which is supposed by the current hash function.
		 */
		virtual size_t getTableSize(void) const {
			return p;
		}

		/**
		 * Sets the size of the table which will be used by the current hash function.
		 *
		 *
		 * @param size New size of the table.
		 */
		virtual void setTableSize(size_t aP) {
			p = aP;
			initializeGenerator();
		}

		/**
		 * Maximum value of the universe retrieval.
		 *
		 * @return Maximal possible value.
		 */
		virtual T getUniversumMax(void) const {
			return p * p - 1;
		}

		/**
		 * Maximum value of the universe.
		 *
		 * @param Maximal possible value.
		 */
		virtual void setUniversumMax(T universumMax) {
		}

		/**
		 * Hash value of the element x
		 *
		 * @param x The hashed element.
		 */
		virtual size_t hash(const T & x) {
			T a = x % p;
			T b = x / p;

			return Hash::Math::UnsignedDoubleWord<T>::add(
				Hash::Math::UnsignedDoubleWord<T>::multiply(a, A, p),
				Hash::Math::UnsignedDoubleWord<T>::multiply(b, B, p),
				p
			);
		}

		/**
		 * Hash value of the element x
		 *
		 * @param x The hashed element.
		 * @param length Length of the table.
		 */
		virtual size_t operator()(const T & a) {
			return hash(a);
		}

		/**
		 * Sets the storage for the hash function.
		 *
		 * @param aStorage Storage using this function.
		 */
		virtual void setStorage(Storage *) {
		}

		class Generator {
		public:
			explicit Generator(T p):
				f(p) {
				f.A = 0;
				f.B = 0;
				f.p = p;
			}

			bool hasNext(void) const {
				return f.A < f.p;
			}

			BadLinearSystem<T, Storage> next() {
				BadLinearSystem<T, Storage> f1 = f;
				++f.B;
				if (f.B == f.p) {
					++f.A;
					f.B = 0;
				}
				return f1;
			}

		private:
			BadLinearSystem<T, Storage> f;
		};

		virtual void swap(BadLinearSystem & function) {
			std::swap(p, function.p);
			std::swap(A, function.A);
			std::swap(B, function.B);
			std::swap(g, function.g);
		}

		virtual std::string toString() const {
			std::stringstream str;
			str << "BadLinear((" << A << ", " << B << ")/" << p << ")";
			return str.str();
		}

	protected:
		virtual void initializeGenerator() {
			g = Hash::Utils::IntegralGeneratorWrapper<T>(0, p - 1);
		}

		T p;
		T A, B;
		Hash::Utils::IntegralGeneratorWrapper<T> g;
	};

} }


namespace std {

	template <typename T, class Storage>
	void swap(Hash::Systems::BadLinearSystem<T, Storage> & a, Hash::Systems::BadLinearSystem<T, Storage> & b) {
		a.swap(b);
	}

	template <typename T, class Storage>
	std::string to_string(const Hash::Systems::BadLinearSystem<T, Storage> & a) {
		return a.toString();
	}

}


#endif
