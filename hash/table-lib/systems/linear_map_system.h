#ifndef LINEAR_MAP_SYSTEM_H
#define LINEAR_MAP_SYSTEM_H

namespace Hash {

	/**
	 * Linear maps.
	 */
	template <typename T>
	class UniversalFunctionLinearMap {
	public:
		UniversalFunctionLinearMap(void):
		}

		size_t getFunctionNo(void) const {
			// TODO: Implement.
			return 0;
		}

		size_t hash(const T & x, size_t length) {
			
		}

		size_t operator()(const T & a, size_t length) {
			
		}

	private:
		size_t universumMax;
		size_t a, b;
	};

}

#endif /* LINEAR_MAP_SYSTEM_H */
