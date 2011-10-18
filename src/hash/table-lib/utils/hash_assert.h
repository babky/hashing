#ifndef ASSERT_H
#define ASSERT_H

#include "config.h"
#include <string>
#include <stdexcept>

namespace Hash { namespace Utils {

	class AssertException : public std::exception {
	public:
		AssertException(std::string message, std::string file, size_t line);
		const char * what(void) const throw();
		
		virtual ~AssertException(void) throw();

	private:
		std::string message;
	};

	void hash_assert(bool status, std::string message, std::string file, size_t line);

#ifdef HASH_DEBUG
	// TODO: Aj pre GCC.
	#ifndef __GNUC__
		#define simple_assert(status, message) Hash::Utils::hash_assert(status, message, __FILE__, __LINE__);
	#else
		#define simple_assert(status, message) ;
	#endif
#else
	#define simple_assert(status, message) ;
#endif

} }

#endif /* ASSERT_H */