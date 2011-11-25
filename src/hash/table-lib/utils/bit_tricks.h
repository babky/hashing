#ifndef BIT_TRICKS_H
#define BIT_TRICKS_H

#include <popcntintrin.h>
#include <cstdint>

namespace Hash { namespace Math {

template<class T> inline
size_t parity(T x) {
	size_t s = sizeof(T);
	simple_assert( s<=sizeof(size_t) && (s==4 || s==8), "Unexpected architecture encountered" );
	return parity_popcnt(x);
}

template<class T> inline
size_t parity_mul(T x) {
	/**	Using one multiplication to save some XORing */
	using namespace std;
	size_t ones = (size_t)0x1111111111111111;
	size_t summask = 0x10 << ((sizeof(T)-1)*8);
	size_t p = x;
	p = p ^ (p >> 1);
	p = (p ^ (p >> 2)) & ones; // now every hex contains its former parity
	p = p * ones; // cumulative summing
	return !!(p & summask);
}

template<class T> inline
size_t parity_shifts(T x) {
	/**	Repeatedly XORing the upper half into the lower half */
	const int bits = 8 * sizeof(T); // not very clean
	size_t p = x; // assuming size_t is the fastest type
	for (int b=bits/2; b!=0; b/=2)
		p ^= (p>>b);
	return p&1;
}

template<class T> inline
size_t parity_popcnt(T x) {
	/**	On CPUs supporting SSE4.2 or SSE4a we have POPCNT instruction */
	return 1 & ( sizeof(T)==8
		? _mm_popcnt_u64(x)
		: _mm_popcnt_u32(x)
	);
}

} } // namespaces

#endif
