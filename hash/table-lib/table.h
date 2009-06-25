#ifndef TABLE_H
#define TABLE_H

#include "function.h"

namespace Hash {

	/**
	 * Hashing table - basic implementation.
	 */
	template<typename T, template <class> class HashFunction>
	class Table {
	public:
		/**
		 * Constructor.
		 */
		Table(void) {
		}

		/**
		 * Inserts the {@code element} into the table.
		 *
		 * @param element Added element.
		 */
		void insert(const T & element) {
			element;
		}

		/**
		 * Removes the {@code element} from the table.
		 *
		 * @param element Removed element.
		 */
		bool remove(const T & element) {
			return false;
		}

		/**
		 * The size of the table retrieval.
		 *
		 * @return Number of hashed elements.
		 */
		int size(void) const {
			return 0;
		}

		/**
		 * Containment check.
		 *
		 * @param element Element to be checked for.
		 * @return Containment check.
		 */
		bool contains(const T & element) const {
			return false;
		}

	protected:
		/**
		 * Used hashed function.
		 */
		HashFunction<T> function;

	private:
		// TODO: Enable it.
		/**
		 * Forbidden copy c-tor.
		 */
		Table(const Table &);

		// TODO: Enable it.
		/**
		 * Forbidden assignment operator.
		 */
		Table & operator =(const Table &);

	};

}

#endif /* TABLE_H */
