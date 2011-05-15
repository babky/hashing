#ifndef CONFIG_H
#define CONFIG_H

#include <boost/config.hpp>

#ifdef BOOST_MSVC
	#ifdef _DEBUG
		#define HASH_DEBUG
	#else
		#define HASH_OPTIMIZE
	#endif
#endif

#ifdef __GNUC__
	#ifdef __OPTIMIZE__ 
		#define HASH_OPTIMIZE
	#else
		#define HASH_DEBUG
	#endif
#endif

#endif /* CONFIG_H */
