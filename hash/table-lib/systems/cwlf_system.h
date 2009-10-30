#ifndef CWLF_SYSTEM_H
#define CWLF_SYSTEM_H

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
			if (!Hash::Utils::StaticRandomGenerator<size_t>::isInitialized()) {
				Hash::Utils::StaticRandomGenerator<size_t>::initialize(0, this->universumMax - 1);
		    }
			
			Hash::Utils::RandomGenerator<size_t> & g = Hash::Utils::StaticRandomGenerator<size_t>::getGenerator();

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

}

#endif /* CWLF_SYSTEM */
