#include "double_word.h"
#include <boost/config.hpp>
#ifdef BOOST_MSVC
	#pragma warning(disable: 4512 4127 4100)
#endif
#include <boost/integer_traits.hpp>
#include <boost/integer.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512 4127 4100)
#endif

using namespace Hash::Math;

// Primes taken from http://primes.utm.edu/lists/2small/0bit.html.

template<>
const size_t Prime<size_t>::GREATEST_PRIME = boost::integer_traits<size_t>::const_max - 4;

#if __LP64__ != 1

template<>
const boost::uint_fast64_t Prime<boost::uint_fast64_t>::GREATEST_PRIME = boost::integer_traits<boost::uint_fast64_t>::const_max - 58;

#endif


