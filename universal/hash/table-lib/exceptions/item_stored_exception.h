#ifndef ITEM_STORED_EXCEPTION_H
#define ITEM_STORED_EXCEPTION_H

#include <exception>
#include "utils/definitions.h"

namespace Hash {

	/**
	 * Exception thrown when inserted item is already in the collection.
	 */
	template<typename T>
	class ItemStoredException : public std::exception {
	public:
		/**
		 * Item stored c-tor. 
		 *
		 * @param item Inserted item causing this exception.
		 */
		ItemStoredException(const T & item) {
			this->item = item;
		}

		/**
		 * Inserted item retrieval.
		 *
		 * @return Inserted item causing this exception.
		 */
		virtual T & getItem(void) {
			return this->item;
		}
		
		virtual const char * what(void) const throw() {
			return "Item already stored in the hash table.";
		}

	private:
		/**
		 * Item already contained.
		 */ 
		T item;
	};

}

#endif /* ITEM_STORED_EXCEPTION_H */
