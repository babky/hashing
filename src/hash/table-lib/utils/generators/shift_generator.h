#ifndef SHIFT_GENERATOR_H
#define SHIFT_GENERATOR_H

namespace Hash { namespace Utils { namespace Generators {

	/**
	 * A simple linear generator combined with half the size T shift.
	 */
	template<typename T>
	class ShiftGenerator {
	public:
		/**
		 * C-tor.
		 */
		ShiftGenerator(void):
			x(0),
			shift(sizeof(T) * 4) {
		}

		/**
		 * Sets the generator to beginning.
		 */
		void reset(void) {
			x = 0;
		}

		/**
		 * Generates the next element.
		 */
		T next(void) {
			T retVal = (x << shift) | (x >> (sizeof(x) * 8 - shift));
			++x;
			return retVal;
		}

	protected:
		/**
		 * Data.
		 */
		T x;

		/**
		 * Shift
		 */
		size_t shift;
	};

} } }

#endif /* SHIFT_GENERATOR_H */
