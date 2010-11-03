#include "hash_math.h"
#include "hash_assert.h"

using namespace Hash::Math;

unsigned int Hash::Math::log2ceil(unsigned int i) {
	unsigned int l = 0;

	for (unsigned int x = 1; x < i; x *= 2) {
		++l;
	}

	return l;
}

unsigned int Hash::Math::log2floor(unsigned int i) {
	unsigned int l = log2ceil(i);
	if (i == static_cast<unsigned int> (1) << l) {
		return l;
	} else {
		return l / 2;
	}
}

unsigned int Hash::Math::log2exact(unsigned int i) {
	unsigned int l = log2ceil(i);
	simple_assert(i == static_cast<unsigned int> (1) << l, "Can not compute exact logarithm.");
	return l;
}
