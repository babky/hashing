#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include "utils/boost_include.h"

namespace Hash { namespace Utils {

	/**
	 * Random generator.
	 *
	 * @param IntType Type of the generated value.
	 */
	template<typename IntType>
	class RandomGenerator {
	public:
		/**
		 * Generating engine.
		 */
		typedef boost::mt19937 Engine;

		/**
		 * Distribution for the random values.
		 */
		typedef boost::uniform_int<IntType> Distribution;

		/**
		 * Generator.
		 */

		typedef boost::variate_generator<Engine&, Distribution> Generator;
		/**
		 * Generator constructor.
		 *
		 * @param min Minimum value.
		 * @param max Maximum value.
		 * @param initializeSeed Automatic (based on current timestamp) seed initialization.
		 */
		RandomGenerator(IntType min, IntType max):
		  generator(Generator(engine, Distribution(min, max))) {
			if (!seedInitialized) {
				using namespace boost::posix_time;
				using namespace boost::gregorian;

				ptime now = second_clock::local_time();
				time_duration time = now.time_of_day();
				generator.engine().seed(time.total_seconds());
				seedInitialized = true;
			}
		}

		/**
		 * Sets the seed.
		 *
		 * @param seed New seed.
		 */
		void setSeed(int seed) {
			generator.engine().seed(seed);
		}

		/**
		 * Generates a new number.
		 *
		 * @param Returns the next random number.
		 */
		IntType generate(void) {
			return this->generator();
		}

		void setDistribution(IntType min, IntType max) {
			typename Generator::distribution_type * distribution = generator.distribution();
			distribution.param(Distribution::param_type(min, max));
		}

	private:
		RandomGenerator(const RandomGenerator &);
		RandomGenerator & operator =(const RandomGenerator &);

		Generator generator;
		static Engine engine;
		static bool seedInitialized;
	};

#ifdef __GNUC__
#ifdef __LP64__
	/**
	 * Random generator for 128 bits.
	 */
	template<>
	class RandomGenerator<__uint128_t> {
	public:
		typedef __uint128_t IntType;

		/**
		 * Generating engine.
		 */
		typedef boost::mt19937 Engine;

		/**
		 * Distribution for the random values.
		 */
		typedef boost::uniform_int<boost::uint64_t> Distribution;

		/**
		 * Generator.
		 */
		typedef boost::variate_generator<Engine&, Distribution> Generator;

		/**
		 * Generator constructor.
		 *
		 * @param min Minimum value.
		 * @param max Maximum value.
		 * @param initializeSeed Automatic (based on current timestamp) seed initialization.
		 */
		RandomGenerator(IntType min, IntType max):
		  engine(Engine()),
		  generator(Generator(engine, Distribution(min, max))) {
			if (!seedInitialized) {
				using namespace boost::posix_time;
				using namespace boost::gregorian;

				ptime now = second_clock::local_time();
				time_duration time = now.time_of_day();
				generator.engine().seed(time.total_seconds());
				seedInitialized = true;
			}
		}

		/**
		 * Sets the seed.
		 *
		 * @param seed New seed.
		 */
		void setSeed(int seed) {
			generator.engine().seed(seed);
		}

		/**
		 * Generates a new number.
		 *
		 * @param Returns the next random number.
		 */
		IntType generate(void) {
			return static_cast<__uint128_t> (this->generator()) << HALF_SIZE | this->generator();
		}

	private:
		static const __uint128_t HALF_SIZE = boost::integer_traits<__uint128_t>::digits / 2;

		RandomGenerator(const RandomGenerator &);
		RandomGenerator & operator =(const RandomGenerator &);

		Generator generator;
		static Engine engine = Engine();
		static bool seedInitialized;
	};

#endif
#endif

} }

#endif /* RANDOM_GENERATOR_H */
