#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include "utils/definitions.h"
#include "utils/storage_statistics.h"

namespace Hash {

	/**
	 * Hashing table - basic implementation.
	 *
	 * @typeparam T Stored item type.
	 * @typeparam EqualityComparer Type of the equality comparer used.
	 * @typeparam HashFunction Function used to determine hash of the item.
	 * @typeparam Storage Storage used for creating the table.
	 */
	template<
		typename T, 
		typename Comparer,
		template <class> class Function, 
		template <class, class, class> class Storage
	>
	class Table : public HashTable<T> {
	public:
		/**
		 * Type for the hash function.
		 */
		typedef Function<T> HashFunction;

		/**
		 * Type for the hash. This is the type of the value returned by the hash function.
		 */
		typedef typename Function<T>::HashType HashType;
		
		/**
		 * Equality comparer.
		 */
		typedef Comparer EqualityComparer;

		/**
		 * Storage - hash table implementation.
		 */
		typedef Storage<T, EqualityComparer, HashType> HashStorage;

		/**
		 * Constructor.
		 */
		Table(void) {
		}

		/**
		 * Constructor.
		 *
		 * @param comparer Comparer used to determine the equality of items.
		 */
		explicit Table(const EqualityComparer & comparer):
			storage(StorageType(comparer)) {
		}

		/**
		 * Inserts the {@code element} into the table.
		 *
		 * @param element Added element.
		 */
		void insert(const T & element) {
			this->storage.insert(element, this->function(element, this->storage.getTableSize()));
		}

		/**
		 * Removes the {@code element} from the table.
		 *
		 * @param element Removed element.
		 */
		bool remove(const T & element) {
			return this->storage.remove(element, this->function(element, this->storage.getTableSize()));
		}

		/**
		 * The size of the table retrieval.
		 *
		 * @return Number of hashed elements.
		 */
		int getSize(void) const {
			return this->storage.getSize();
		}

		/**
		 * Containment check.
		 *
		 * @param element Element to be checked for.
		 * @return Containment check.
		 */
		bool contains(const T & element) const {
			return this->storage.contains(element, this->function(element, this->storage.getTableSize()));
		}
		
		/**
		 * Clears the stored set.
		 */
		void clear(void) {
			this->storage.clear();
		}

		/**
		 * Load factor retrieval.
		 *
		 * @return Load factor.
		 */
		double getLoadFactor(void) const {
			return this->storage.getLoadFactor();
		}

		/**
		 * Computes the statistics for this storage.
		 *
		 * @param stats Storage for statistics.
		 */ 
		void computeStatistics(Utils::StorageStatistics & stats) const {
			this->storage.computeStatistics(stats);
		}

	protected:
		/**
		 * Used hashed function.
		 */
		mutable HashFunction function;

		/**
		 * Used storage.
		 */
		HashStorage storage;

	};

}

#endif /* TABLE_H */
