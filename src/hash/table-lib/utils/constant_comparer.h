#ifndef CONSTANT_COMPARER_H
#define CONSTANT_COMPARER_H

#include <algorithm>

namespace Hash { namespace Utils {

	/**
	 * Comparer that always returns the predefined result. The comparison result is set in the constructor and can not 
	 * be changed afterwards. Useful for stastics gathering storages.
	 */
	template<typename T>
	class ConstantComparer {
	public:
		/**
		 * False result always adds an element into a chain and never deletes it.
		 */
		static const bool COMPARISON_RESULT = false;

		/**
		 * C-tor.
		 *
		 * @param resultState The comparison result always returned by the created comparer.
		 */
		explicit ConstantComparer(bool resultState = COMPARISON_RESULT):
		  state(resultState) {
		}

		/**
		 * Compares the items.
		 *
		 * @param a Argument to be compared.
		 * @param b Argument to be compared.
		 * @return The predefined result.
		 */
		bool operator()(const T & a, const T & b) {
			return getState();
		}

		/**
		 * Comparison result.
		 * 
		 * @return The given comparison result set in the c-tor.
		 */
		bool getState() const {
			return state;
		}

	private:
		/**
		 * The comparison result always returned by the created comparer.
		 */
		bool state;
	};

} }

#endif /* CONSTANT_COMPARER_H */