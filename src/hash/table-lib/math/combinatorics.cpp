#include "combinatorics.h"

unsigned long long Hash::Math::binomial_coefficient(size_t n, size_t k) {
	unsigned long long up = 1, down = 1;
	for (int i = 0; i < k; ++i) {
		up *= (n - i);
		down *= (k - i);
	}
	return up / down;
}
