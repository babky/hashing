#ifndef IDENTITY_FUNCTION
#define IDENTITY_FUNCTION

#include <boost/config.hpp>
#ifdef BOOST_MSVC
	#pragma warning(disable: 4512 4127 4100)
#endif
#include <boost/integer.hpp>
#include <boost/integer_traits.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512 4127 4100)
#endif

#include "universal_system.h"
#include <cstdlib>
#include <algorithm>

namespace Hash { namespace Systems {

	template<typename T, class Storage>
	class IdentityFunction : public UniversalFunction<T, Storage> {
	public:
		explicit IdentityFunction(size_t size = 32) {
			setTableSize(size);
		}

		void reset(void) {
		}

		size_t getTableSize(void) const {
			return mask + 1;
		}
		
		void setTableSize(size_t size) {
			mask = size - 1;
		}

		T getUniversumMax(void) const {
			return boost::integer_traits<T>::const_max;
		}

		void setUniversumMax(T) {
		}

		size_t hash(const T & x) {
			return x & mask;
		}

		size_t operator()(const T & a) {
			return hash(a);
		}

		void swap(IdentityFunction & a) {
			using std::swap;
			swap(mask, a.mask);
		}

	protected:
		size_t mask;
	};

} }

#endif /* IDENTITY_FUNCTION */
