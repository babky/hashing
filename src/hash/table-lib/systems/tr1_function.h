#ifndef TR1_FUNCTION
#define TR1_FUNCTION

#include "../utils/boost_include.h"
#include "universal_system.h"
#include <cstdlib>
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
			std::ldiv_t _Qrem = std::ldiv((long)(size_t)x, 127773);

			_Qrem.rem = 16807 * _Qrem.rem - 2836 * _Qrem.quot;
			if (_Qrem.rem < 0)
				_Qrem.rem += 2147483647;
			return ((size_t)_Qrem.rem) & mask;
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
