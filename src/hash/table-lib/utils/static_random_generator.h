#ifndef STATIC_RANDOM_GENERATOR_H
#define STATIC_RANDOM_GENERATOR_H

#include "random_generator.h"
#include "utils/boost_include.h"
#include "utils/smart_pointer.h"

namespace Hash { namespace Utils {

	template<typename T>
	class StaticRandomGenerator {
	public:
		static Hash::Utils::RandomGenerator<T> & getGenerator(void) {
			if (!isInitialized()) {
				initialize();
			}

			return *generator;
		}

		static void initialize(T min = boost::integer_traits<T>::const_min, T max = boost::integer_traits<T>::const_max) {
			StaticRandomGenerator<T>::generator = new RandomGenerator<T>(min, max, true);
		}

		static bool isInitialized(void) {
			return StaticRandomGenerator<T>::generator != 0;
		}

	private:
		static RandomGenerator<T> * generator;
	}; 

	template<typename T>
	class IntegralGeneratorWrapper {
	public:
		IntegralGeneratorWrapper(T minimum, T maximum):
			min(minimum), 
			max(maximum),
			g(new RandomGenerator<size_t>(minimum, maximum, true)) {
		}

		T getMin(void) const {
			return min;
		}

		T getMax(void) const {
			return max;
		}

		T generate(void) {
			return g->generate();
		}

	private:
		T min;
		T max;
		Hash::Utils::SmartPointer<Hash::Utils::RandomGenerator<size_t>> g;
	};
	 
} }

#endif /* STATIC_RANDOM_GENERATOR_H */
