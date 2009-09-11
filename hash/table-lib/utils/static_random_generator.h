#ifndef STATIC_RANDOM_GENERATOR_H
#define STATIC_RANDOM_GENERATOR_H

#include "random_generator.h"

namespace Hash { namespace Utils {

	template<typename T>
	class StaticRandomGenerator {
	public:
		static Hash::Utils::RandomGenerator<T> & getGenerator(void) {
			return *generator;
		}

		static void initialize(T min, T max) {
			StaticRandomGenerator<T>::generator = new RandomGenerator<T>(min, max, true);
		}

		static bool isInitialized(void) {
			return StaticRandomGenerator<T>::generator != 0;
		}

	private:
		static RandomGenerator<T> * generator;
	}; 
	 
} }

#endif /* STATIC_RANDOM_GENERATOR_H */
