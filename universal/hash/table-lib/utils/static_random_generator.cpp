#include "static_random_generator.h"

using namespace Hash::Utils;

RandomGenerator<size_t> * StaticRandomGenerator<size_t>::generator = 0;
RandomGenerator<int> * StaticRandomGenerator<int>::generator = 0;
RandomGenerator<boost::uint_fast64_t> * StaticRandomGenerator<boost::uint_fast64_t>::generator = 0;
RandomGenerator<boost::int_fast64_t> * StaticRandomGenerator<boost::int_fast64_t>::generator = 0;
