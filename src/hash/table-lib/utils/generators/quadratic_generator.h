#ifndef QUADRATIC_GENERATOR_H
#define QUADRATIC_GENERATOR_H

namespace Hash { namespace Utils { namespace Generators {

	/**
	 * A simple quadratic generator.
	 */
	template<typename T>
	class QuadraticGenerator {
	public:
		/**
		 * C-tor.
		 */
		QuadraticGenerator(void):
			i(0) {
		}

		/**
		 * Sets the generator to beginning.
		 */
		void reset(void) {
			i = 0;
		}

		/**
		 * Generates the next element.
		 */
		T next(void) {
			T retVal = i * i;
			++i;
			return retVal;
		}

	protected:
		/**
		 * Data.
		 */
		T i;
	};

} } }

#endif /* QUADRATIC_GENERATOR_H */
