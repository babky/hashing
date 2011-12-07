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

#ifdef __GNUC__
namespace boost {
	template<>
	struct integer_traits<__uint128_t> {
		static const __uint128_t const_min = 0;
		static const __uint128_t const_max = -1;
		static const size_t digits = 128;
	};
}
#endif

#ifdef BOOST_MSVC
        #pragma warning(default: 4512 4127 4100)
#endif

#endif /* BOOST_INCLUDE_H */
