#ifndef HASH_MATH_H
#define HASH_MATH_H

#include "hash_assert.h"

namespace Hash { namespace Math {

	template <typename T>
	unsigned int log2ceil(T i) {
		unsigned int l = 0;

		for (T x = 1; x && (x < i); x *= 2) {
			++l;
		}

		return l;
	}

    template <>
    unsigned int log2ceil<unsigned int>(unsigned int i);

	template <typename T>
	unsigned int log2floor(T i) {
		unsigned int l = log2ceil(i);
		if (i == (static_cast<T> (1)) << l) {
			return l;
		} else {
			return l - 1;
		}
	}

    template <>
    unsigned int log2floor<unsigned int>(unsigned int i);

	template <typename T>
	unsigned int log2exact(T i) {
		unsigned int l = log2ceil(i);
		simple_assert(i == (static_cast<T> (1)) << l, "Can not compute exact logarithm.");
		return l;
	}

    template <>
    unsigned int log2exact<unsigned int>(unsigned int i);

	template <typename T>
	bool is_power_of_2(T i) {
		unsigned int l = log2ceil(i);
		return i == (static_cast<T> (1)) << l;
	}

	template <typename T>
	unsigned int next_power(T i) {
		unsigned int l = log2ceil(i);
		return (static_cast<T> (1)) << l;
	}
} }

#endif /* HASH_MATH_H */
