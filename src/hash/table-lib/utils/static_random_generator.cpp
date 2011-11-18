#include "static_random_generator.h"

using namespace Hash::Utils;

template<>
RandomGenerator<boost::uint32_t> * StaticRandomGenerator<boost::uint32_t>::generator = 0;

template<>
RandomGenerator<int> * StaticRandomGenerator<int>::generator = 0;

template<>
RandomGenerator<unsigned char> * StaticRandomGenerator<unsigned char>::generator = 0;

template<>
RandomGenerator<boost::uint64_t> * StaticRandomGenerator<boost::uint64_t>::generator = 0;

