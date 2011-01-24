#ifndef STATIC_RANDOM_GENERATOR_H
#define STATIC_RANDOM_GENERATOR_H

#include "random_generator.h"
#ifdef BOOST_MSVC
	#pragma warning(disable: 4512 4127 4100)
#endif
#include <boost/integer_traits.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512 4127 4100)
#endif

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

		static void initialize(T min, T max) {
			StaticRandomGenerator<T>::generator = new RandomGenerator<T>(min, max, true);
		}

		static bool isInitialized(void) {
			return StaticRandomGenerator<T>::generator != 0;
		}

	private:
		static RandomGenerator<T> * generator;
	}; 

	
	template<>
	class StaticRandomGenerator<size_t> {
	public:
		static Hash::Utils::RandomGenerator<size_t> & getGenerator(void) {
			if (!isInitialized()) {
				initialize();
			}

			return *generator;
		}

		static void initialize(size_t min = 0, size_t max = boost::integer_traits<size_t>::const_max) {
			StaticRandomGenerator<size_t>::generator = new RandomGenerator<size_t>(min, max, true);
		}

		static bool isInitialized(void) {
			return StaticRandomGenerator<size_t>::generator != 0;
		}

	private:
		static RandomGenerator<size_t> * generator;
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
