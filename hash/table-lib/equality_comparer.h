#ifndef EQUALITY_COMPARER_H
#define EQUALITY_COMPARER_H

namespace Hash { namespace Utils {

	/**
	 * Default equality comparer.
	 */
	template <typename T>
	class EqualityComparer {

		/**
		 * Equality test predicate.
		 *
		 * @param a	Argument to be checked for equality.
		 * @param b Argument to be checked for equality.
		 * @return Equality status.
		 */
		public bool operator()(const T & a, const T & b) {
			return a == b;
		}

	};

} }

#endif /* EQUALITY_COMPARER_H */
