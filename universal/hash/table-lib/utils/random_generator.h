#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#ifdef BOOST_MSVC
	#pragma warning(disable: 4512 4127 4100)
#endif
#include <boost/config.hpp>
#include <boost/date_time.hpp>
#include <boost/random.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512 4127 4100)
#endif

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
		explicit RandomGenerator(IntType min, IntType max, bool initializeSeed = false):
		  engine(Engine()),
		  generator(Generator(engine, Distribution(min, max))) {
			if (initializeSeed) {
				using namespace boost::posix_time;
				using namespace boost::gregorian;

				ptime now = second_clock::local_time();
				time_duration time = now.time_of_day();
				generator.engine().seed(time.total_seconds());
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

	private:
		RandomGenerator(const RandomGenerator &);
		RandomGenerator & operator =(const RandomGenerator &);

		Generator generator;
		Engine engine;
	};

} }


#endif /* RANDOM_GENERATOR_H */
