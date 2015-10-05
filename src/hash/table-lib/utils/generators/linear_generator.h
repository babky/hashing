#ifndef LINEAR_GENERATOR_H
#define LINEAR_GENERATOR_H

namespace Hash { namespace Utils { namespace Generators {

	/**
	 * A simple linear generator.
	 */
	template<typename T>
	class LinearGenerator {
	public:
		/**
		 * C-tor.
		 */
		LinearGenerator(void):
			x(0) {
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
			return x++;
		}

	protected:
		/**
		 * Data.
		 */
		T x;
	};

} } }

#endif /* LINEAR_GENERATOR_H */
