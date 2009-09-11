#ifndef UNIVERSAL_FUNCTION_H
#define UNIVERSAL_FUNCTION_H

#include "utils/random_generator.h"

namespace Hash {

	/**
	 * Carter - Wegman system of linear functions.
	 */
	template <typename T>
	class UniversalFunctionCWLF {
	public:
		const static size_t UNIVERSUM_MAX_VALUE = 65537;

		typedef size_t HashType;
		UniversalFunctionCWLF(void):
		  universumMax(UNIVERSUM_MAX_VALUE) {
			  Hash::Utils::RandomGenerator<size_t> g(0, this->universumMax - 1, true);

			this->a = g.generate();
			this->b = g.generate();
		}

		size_t getFunctionNo(void) const {
			return this->a * this->universumMax + this->b;
		}

		size_t hash(const T & x, size_t length) {
			return ((this->a * x + this->b) % this->universumMax) % length;
		}

		size_t operator()(const T & a, size_t length) {
			return hash(a, length);
		}

	private:
		size_t universumMax;
		size_t a, b;
	};

	/**
	 * Omega strongly universal system of functions.
	 */
	// TODO: Implement it.
	template <typename T>
	class UniversalFunctionOmega {
	public:
		typedef size_t HashType;

		UniversalFunctionOmega(void) {
		}

		size_t getFunctionNo(void) const {
			return this->functionNo;
		}

	private:
		size_t functionNo;

		size_t a, b;
	};

}

#endif /* UNIVERSAL_FUNCTION_H */
