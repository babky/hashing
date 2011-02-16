#ifndef OMEGA_SYSTEM_H
#define OMEGA_SYSTEM_H

namespace Hash {

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

		void initialize(Hash::StorageInfo &) {
		}
			
	private:
		size_t functionNo;

		size_t a, b;
	};

}

#endif /* OMEGA_SYSTEM_H */
