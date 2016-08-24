#ifndef HASH_MATH_H
#define HASH_MATH_H

#include "hash_assert.h"

namespace Hash { namespace Math {

	template <typename T>
	unsigned int log2ceil(T i) {
		unsigned int l = 0;

		for (T x = 1; x < i; x *= 2) {
			++l;
		}

		return l;
	}

	template <typename T>
	unsigned int log2floor(T i) {
		unsigned int l = log2ceil(i);
		if (i == (static_cast<T> (1)) << l) {
			return l;
		} else {
			return l / 2;
		}
	}

	template <typename T>
	unsigned int log2exact(T i) {
		unsigned int l = log2ceil(i);
		simple_assert(i == (static_cast<T> (1)) << l, "Can not compute exact logarithm.");
		return l;
	}
} }

#endif /* HASH_MATH_H */
