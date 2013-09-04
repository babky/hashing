#ifndef DOUBLE_WORD_H
#define DOUBLE_WORD_H

#include "../utils/boost_include.h"

namespace Hash { namespace Math {

	template<typename T>
	struct Prime {
		static const T GREATEST_PRIME;
	};

	template<typename UnsignedWord>
	class UnsignedDoubleWord;

	template<typename T>
	T exponentiate(const T a, T e, const T m) {
		T res = a;
		e -= 1;

		while (e > 0) {
			res = UnsignedDoubleWord<T>::multiply(res, res, m);

			if (e % 2 == 1) {
				res = UnsignedDoubleWord<T>::multiply(res, a, m);
			}

			e /= 2;
		}

		return res;
	}

	template<typename UnsignedWord>
	class UnsignedDoubleWord {
	public:
		typedef UnsignedWord Word;

		inline static Word multiply(const Word x, const Word y, const Word m) {
			// Do it like (xu + xl) * (yu + yl) = xu * yu + (xl * yu + xu * yl) + xl * yl.

			// Split x into xl and xu.
			Word xu = x >> WORD_BITS_HALF;
			Word xl = ((Word) (x << WORD_BITS_HALF)) >> WORD_BITS_HALF;

			// Split y into yl and yu.
			Word yu = y >> WORD_BITS_HALF;
			Word yl = ((Word) (y << WORD_BITS_HALF)) >> WORD_BITS_HALF;

			// Notice:
			// 1) xl * yu + xu * yl may overflow the buffer
			// 2) xl * yl may not overflow the buffer
			// 3) 1 + xu * yu may not overflow the buffer since (2^w - 1) * (2^w - 1) + 1 < 2^2w

			// 11 * 11 = 1001 4bits max
			Word l = xl * yl;
			Word u = xu * yu;

			Word m1 = xu * yl;
			Word m2 = xl * yu;
			Word m3 = (u << WORD_BITS_HALF) | (l >> WORD_BITS_HALF);

			// Possible carry, placed at the right position.
			Word c = carry(m3, m2) >> (WORD_BITS_HALF - 1);
			m2 += m3;

			// Possible carry, placed at the right position also after sum with the previous value.
			c += carry(m1, m2) >> (WORD_BITS_HALF - 1);
			m1 += m2;

			u = ((((Word) (u >> WORD_BITS_HALF)) << WORD_BITS_HALF) | (m1 >> WORD_BITS_HALF)) + c;
			l = (((Word) (l << WORD_BITS_HALF)) >> WORD_BITS_HALF) | (m1 << WORD_BITS_HALF);

			return UnsignedDoubleWord<UnsignedWord>::modulo(u, l, m);
		}

		inline static Word add(const Word x, const Word y, const Word m) {
			return UnsignedDoubleWord<UnsignedWord>::modulo(carry(x, y) >> (WORD_BITS - 1), x + y, m);
		}

		inline static Word modulo(const Word u, const Word l, const Word m) {
			// Do it like this.
			// (x + y) % m = x % m + y % m

			// Compute moduli.
			Word um = u % m;
			Word lm = l % m;
			unsigned char addedBit;

			// The division algorithm.
			for (size_t bit = WORD_BITS; bit;) {
				if (um == 0) {
					return lm;
				}

				--bit;
				addedBit = lm & ((Word) 1 << bit);
				lm &= ~addedBit;
				addedBit >>= bit;

				if (um >> (WORD_BITS - 1)) {
					um = um - (m - um) + addedBit;
				} else {
					um = (um << 1) + addedBit;
					um %= m;
				}
			}

			return um;
		}

		inline static Word linear(const Word a, const Word x, const Word b, const Word m) {
			return add(multiply(a, x, m), b, m);
		}

		inline static Word exponentiate(const Word a, const Word e, const Word m) {
			return Hash::Math::exponentiate(a, e, m);
		}

	private:
		inline static Word carry(const Word & x, const Word & y) {
			return ((x >> 1) + (y >> 1) + ((x & 1) & (y & 1))) & ((Word) 1 << (WORD_BITS - 1));
		}

		static const size_t WORD_BITS = boost::integer_traits<Word>::digits;
		static const size_t WORD_BITS_HALF = boost::integer_traits<Word>::digits / 2;
	};

	template <>
	class UnsignedDoubleWord<boost::uint32_t> {
	public:
		typedef boost::uint32_t Word;

		inline static Word multiply(const Word x, const Word y, const Word m) {
			return (static_cast<boost::uint64_t> (x) * static_cast<boost::uint64_t> (y)) % static_cast<boost::uint64_t> (m);
		}

		inline static Word add(const Word x, const Word y, const Word m) {
			return (static_cast<boost::uint64_t> (x) + static_cast<boost::uint64_t> (y)) % static_cast<boost::uint64_t> (m);
		}

		inline static Word linear(const Word a, const Word x, const Word b, const Word m) {
			return static_cast<Word> ((static_cast<boost::uint64_t> (a) * static_cast<boost::uint64_t> (x) + static_cast<boost::uint64_t> (b)) % static_cast<boost::uint64_t> (m));
		}

		inline static Word exponentiate(const Word a, const Word e, const Word m) {
			return Hash::Math::exponentiate(a, e, m);
		}

	};

#ifdef __GNUC__
#ifdef __LP64__
	template <>
	class UnsignedDoubleWord<boost::uint64_t> {
	public:
		typedef boost::uint64_t Word;

		inline static Word multiply(const Word x, const Word y, const Word m) {
			return (static_cast<__uint128_t> (x) * static_cast<__uint128_t> (y)) % static_cast<__uint128_t> (m);
		}

		inline static Word add(const Word x, const Word y, const Word m) {
			return (static_cast<__uint128_t> (x) + static_cast<__uint128_t> (y)) % static_cast<__uint128_t> (m);
		}

		inline static Word linear(const Word a, const Word x, const Word b, const Word m) {
			return static_cast<Word> ((static_cast<__uint128_t> (a) * static_cast<__uint128_t> (x) + static_cast<__uint128_t> (b)) % static_cast<__uint128_t> (m));
		}

		inline static Word exponentiate(const Word a, const Word e, const Word m) {
			return Hash::Math::exponentiate(a, e, m);
		}

	};
#endif
#endif

} }

#endif /* DOUBLE_WORD_H */
