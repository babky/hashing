#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <boost/config.hpp>
#include <boost/date_time.hpp>

#ifdef BOOST_MSVC
	#pragma warning(disable: 4512)
#endif
#include <boost/random.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512)
#endif

namespace Hash { namespace Utils {

	template<typename IntType>
	class RandomGenerator {
	public:
		typedef boost::mt19937 Engine;
		typedef boost::uniform_int<IntType> Distribution;
		typedef boost::variate_generator<Engine&, Distribution> Generator;

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
