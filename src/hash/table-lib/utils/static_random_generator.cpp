#include "static_random_generator.h"

using namespace Hash::Utils;

template<>
RandomGenerator<boost::uint32_t> * StaticRandomGenerator<boost::uint32_t>::generator = 0;
template<>
bool RandomGenerator<boost::uint32_t>::seedInitialized = false;
template<>
RandomGenerator<boost::uint32_t>::Engine RandomGenerator<boost::uint32_t>::engine = RandomGenerator<boost::uint32_t>::Engine();

template<>
RandomGenerator<int> * StaticRandomGenerator<int>::generator = 0;
template<>
bool RandomGenerator<int>::seedInitialized = false;
template<>
RandomGenerator<int>::Engine RandomGenerator<int>::engine = RandomGenerator<int>::Engine();

template<>
RandomGenerator<unsigned char> * StaticRandomGenerator<unsigned char>::generator = 0;
template<>
bool RandomGenerator<unsigned char>::seedInitialized = false;
template<>
RandomGenerator<unsigned char>::Engine RandomGenerator<unsigned char>::engine = RandomGenerator<unsigned char>::Engine();

template<>
RandomGenerator<boost::uint64_t> * StaticRandomGenerator<boost::uint64_t>::generator = 0;
template<>
bool RandomGenerator<boost::uint64_t>::seedInitialized = false;
template<>
RandomGenerator<boost::uint64_t>::Engine RandomGenerator<boost::uint64_t>::engine = RandomGenerator<boost::uint64_t>::Engine();

#ifdef __GNUC__
#if __LP64__
template<>
RandomGenerator<__uint128_t> * StaticRandomGenerator<__uint128_t>::generator = 0;
#endif
#endif
