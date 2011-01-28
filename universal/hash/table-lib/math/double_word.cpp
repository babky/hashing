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

template<>
const size_t Prime<size_t>::GREATEST_PRIME = (size_t) 4294967291;

template<>
const boost::uint_fast64_t Prime<boost::uint_fast64_t>::GREATEST_PRIME = (boost::uint_fast64_t) 18446744073709551557;


