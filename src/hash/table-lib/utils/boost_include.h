#ifndef BOOST_INCLUDE_H
#define BOOST_INCLUDE_H

#include <boost/config.hpp>
#ifdef BOOST_MSVC
	#pragma warning(disable: 4512 4127 4100)
#endif
#include <boost/date_time.hpp>
#include <boost/random.hpp>
#include <boost/integer.hpp>
#include <boost/integer_traits.hpp>

#ifdef BOOST_MSVC
	#pragma warning(default: 4512 4127 4100)
#endif

#endif /* BOOST_INCLUDE_H */
