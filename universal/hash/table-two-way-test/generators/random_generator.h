#ifndef TWT_RANDOM_GENERATOR_H
#define TWT_RANDOM_GENERATOR_H

#include "generator.h"
#include "utils/static_random_generator.h"

namespace Hash { namespace Generators {

	template <typename T>
	class RandomGenerator : public Generator<T> {
	public:
		static const size_t DEFAULT_SEED = 1;

		RandomGenerator(T aMin, T aMax, bool aAcceptSeed):
		  acceptSeed(aAcceptSeed),
		  generator(aMin, aMax) {
			generator.setSeed(DEFAULT_SEED);
		}

		virtual void setSeed(T aSeed) {
			if (acceptSeed) {
				generator.setSeed(static_cast<int> (aSeed));
			}
		}

		virtual void setFrom(T) {
		}

		virtual void setPartLength(size_t) {
		}

		virtual void setTableLength(size_t) {
		}
		
		virtual void initialize(void) {
		}

		virtual T generate(void) {
			return generator.generate();
		}

	private:
		bool acceptSeed;
		Hash::Utils::RandomGenerator<T> generator;
	};

} }

#endif /* TWT_RANDOM_GENERATOR_H */
