#ifndef LINEAR_GENERATOR_H
#define LINEAR_GENERATOR_H

#include "generator.h"
#include <iostream>

namespace Hash { namespace Generators {

	template <typename T>
	class LinearGenerator : public Generator<T> {
	public:
		LinearGenerator(T aMin, T aMax, bool aAcceptSeed):
		  min(aMin),
		  max(aMax),
		  i(0),
		  seed(1),
		  tableLength(0),
		  acceptSeed(aAcceptSeed) {
		}

		virtual void setSeed(T aSeed) {
			seed = aSeed;
		}

		virtual void setFrom(T from) {
			min = from;
		}

		virtual void setPartLength(size_t) {
		}

		virtual void setTableLength(size_t aTableLength) {
			tableLength = aTableLength;
		}

		virtual void initialize(void) {
			/*if (seed % 2) {
				seed *= tableLength;
			}*/

			if (!acceptSeed) {
				seed = tableLength;
			}
		}

		virtual T generate(void) {
			using namespace std;

			T g = (seed * i++) % (max - min) + min;

			// cout << g << endl;

			return g;
		}

	private:
		T min;
		T max;
		T i;
		T seed;
		size_t tableLength;
		bool acceptSeed;
	};

} }

#endif /* LINEAR_GENERATOR_H */
