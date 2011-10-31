#ifndef TR1_FUNCTION
#define TR1_FUNCTION

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
#include <algorithm>

namespace Hash { namespace Systems {

	template<typename T, class Storage>
	class Tr1Function : public UniversalFunction<T, Storage> {
	public:
		explicit Tr1Function(size_t size = 32) {
			setTableSize(size);
		}

		void reset(void) {
		}

		virtual size_t getTableSize(void) const {
			return mask + 1;
		}
		
		virtual void setTableSize(size_t size) {
			mask = size - 1;
		}

		virtual T getUniversumMax(void) const {
			return boost::integer_traits<T>::const_max;
		}

		virtual void setUniversumMax(T) {
		}

		virtual size_t hash(const T & x) {
			return x & mask;
		}

		virtual size_t operator()(const T & a) {
			return hash(a);
		}

		void swap(Tr1Function & a) {
			using std::swap;
			swap(mask, a.mask);
		}

	protected:
		size_t mask;
	};

} }

#endif /* TR1_FUNCTION */