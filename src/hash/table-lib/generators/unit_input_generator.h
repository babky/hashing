#ifndef UNIT_INPUT_GENERATOR_H
#define UNIT_INPUT_GENERATOR_H

#include "generator.h"

namespace Hash { namespace Generators {

	template <typename T>
	class UnitInputGenerator : public Generator<T> {
	public:
		UnitInputGenerator(T aMin, T aMax, bool aAcceptSeed): 
		  digits(0),
		  min(aMin),
		  max(aMax),
		  i(0),
		  seed(1),
		  tableLength(0),
		  acceptSeed(aAcceptSeed) {
		}

		virtual ~UnitInputGenerator() {
			delete [] digits;
			digits = 0;
		}

		virtual void setSeed(T aSeed) {
			seed = aSeed;
		}
		
		virtual void setFrom(T from) {
			min = from;
		}

		virtual void setPartLength(size_t) {
		}

		virtual void setTableLength(size_t aTableLength) {
			tableLength = aTableLength;
		}

		virtual void initialize(void) {
			if (seed % 2) {
				seed *= tableLength;
			}
			
			if (!acceptSeed) {
				seed = 1;
			}

			digits = new size_t[boost::integer_traits<size_t>::digits];
			for (size_t i = 0; i < boost::integer_traits<size_t>::digits; ++i) {
				digits[i] = 0;
			}

			/*
			for (size_t i = 0; i < min; ++i) {
				generate();
			}
			*/
		}
		
		virtual T generate(void) {
			// Assemble the result.
			T r = 0;
			for (size_t i = 0; i < boost::integer_traits<size_t>::digits; ++i) {
				if (!digits[i]) {
					break;
				}

				r |= (T) 1 << (digits[i] - 1);
			}

			// Increment digits.
			for (size_t i = 0; i < boost::integer_traits<size_t>::digits; ++i) {
				++digits[i];
				if (digits[i] != boost::integer_traits<size_t>::digits + 1 - i) {
					while (i > 0) {
						--i;
						digits[i] = digits[i + 1] + 1;
					}
					break;
				}
			}

			return r;
		}

	private:
		size_t * digits;
		T min;
		T max;
		T i;
		T seed;
		size_t tableLength;
		bool acceptSeed;
	};

} }

#endif /* UNIT_INPUT_GENERATOR_H */
