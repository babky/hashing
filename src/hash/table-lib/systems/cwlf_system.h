#ifndef CWLF_SYSTEM_H
#define CWLF_SYSTEM_H

#include <algorithm>
#include "systems/universal_system.h"
#include "utils/hash_assert.h"
#include "math/double_word.h"
#include "storage.h"
#include "utils/static_random_generator.h"

namespace Hash { namespace Systems {

	/**
	 * Carter - Wegman system of linear functions.
	 */
	template <typename T, class Storage>
	class UniversalFunctionCWLF : public Hash::Systems::UniversalFunction<T, Storage> {
	public:
		explicit UniversalFunctionCWLF(size_t aTableSize = StorageParams::INITIAL_STORAGE_SIZE, size_t aPrime = Hash::Math::Prime<T>::GREATEST_PRIME):
		  prime(aPrime),
		  tableSize(aTableSize) {
			reset();
		}

		void setUniversumMax(T aUniversumMax) {
			prime = aUniversumMax + 1;
		}

		void setPrime(T aPrime) {
			prime = aPrime;
		}

		T getUniversumMax(void) const {
			return prime - 1;
		}

		void setTableSize(size_t size) {
			this->tableSize = size;
		}

		size_t getTableSize(void) const {
			return this->tableSize;
		}

		void reset(void) {
			Hash::Utils::IntegralGeneratorWrapper<T> g = Hash::Utils::IntegralGeneratorWrapper<T>(0, prime);

			a = g.generate();
			b = g.generate();
		}

		size_t hash(const T & x) {
			size_t hv = Hash::Math::UnsignedDoubleWord<T>::linear(a, x, b, prime) % tableSize;
			return hv;
		}

		size_t operator()(const T & x) {
			return hash(x);
		}

		void swap(UniversalFunctionCWLF & function) {
			std::swap(prime, function.prime);
			std::swap(a, function.a);
			std::swap(b, function.b);
			std::swap(tableSize, function.tableSize);
		}

		virtual std::string toString() const {
			std::stringstream str;
			str << "CWLF((" << a << " * x + " << b << ") % " << prime << " % " << tableSize << ")";
			return str.str();
		}

		class Generator {
		public:
			explicit Generator(T aPrime, size_t tableSize, bool aFixConstant = false):
				fixConstant(aFixConstant) {
				f.a = 0;
				f.b = 0;
				f.setPrime(aPrime);
				f.setTableSize(tableSize);
			}

			bool hasNext(void) const {
				return (f.a < (f.prime - 1)) || (!fixConstant && (f.b < (f.prime - 1)));
			}

			UniversalFunctionCWLF<T, Storage> next() {
				// First call.
				if (f.a == 0) {
					f.a = 1;
					return f;
				}

				if (!fixConstant) {
					++f.b;
					if (f.b < f.prime) {
						return f;
					} else {
						f.b = 0;
					}
				}

				++f.a;
				return f;
			}

		private:
			UniversalFunctionCWLF<T, Storage> f;
			bool fixConstant;
		};

	private:
		T prime;
		T a, b;
		size_t tableSize;
	};



} }

namespace std {

	template <typename T, class Storage>
	void swap(Hash::Systems::UniversalFunctionCWLF<T, Storage> & a, Hash::Systems::UniversalFunctionCWLF<T, Storage> & b) {
		a.swap(b);
	}

	// TODO: Make generic interface having toString and the template for this interface.
	template <typename T, class Storage>
	std::string to_string(const Hash::Systems::UniversalFunctionCWLF<T, Storage> & a) {
		return a.toString();
	}

}

#endif /* CWLF_SYSTEM */
