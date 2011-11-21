#include "double_word.h"
#include "utils/boost_include.h"

using namespace Hash::Math;

// Primes taken from http://primes.utm.edu/lists/2small/0bit.html.

template<>
const boost::uint32_t Prime<boost::uint32_t>::GREATEST_PRIME = boost::integer_traits<boost::uint32_t>::const_max - 4;

template<>
const boost::uint64_t Prime<boost::uint64_t>::GREATEST_PRIME = boost::integer_traits<boost::uint64_t>::const_max - 58;

#ifdef __GNUC__
template<>
const __uint128_t Prime<__uint128_t>::GREATEST_PRIME = ((__uint128_t) 0) - 158;
#endif
