#ifndef STATIC_RANDOM_GENERATOR_H
#define STATIC_RANDOM_GENERATOR_H

#include "random_generator.h"
#include "utils/boost_include.h"

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
			max(maximum) {
			this->g = &StaticRandomGenerator<size_t>::getGenerator();
		}

		T getMin(void) const {
			return this->min;
		}

		T getMax(void) const {
			return this->max;
		}

		T generate(void) const {
			return this->g->generate() % (this->max - this->min) + this->min;
		}

	private:
		T min;
		T max;

		Hash::Utils::RandomGenerator<size_t> * g;
	};
	 
} }

#endif /* STATIC_RANDOM_GENERATOR_H */
