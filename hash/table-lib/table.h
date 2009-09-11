#ifndef TABLE_H
#define TABLE_H

#include <algorithm>
#include "utils/definitions.h"
#include "utils/storage_statistics.h"
#include "policies/rehash_policy.h"

namespace Hash {

	/**
	 * Rehashing operation to be done.
	 */ 
	enum RehashingOperation {
		Refresh,
		Shrink,
		Enlarge
	};

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
		template <class, class, class> class Storage,
		class RehashPolicy = Policies::Rehash::LoadFactorBoundsRehashPolicy
	>
	class Table : public HashTable<T> {
	public:
		/**
		 * Own type.
		 */
		typedef Table<T, Comparer, Function, Storage, RehashPolicy> HashTable;

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
		 * Iterator for iterating throughout the items of the table.
		 */
		typedef typename HashStorage::Iterator Iterator;

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
			storage(HashStorage(comparer)) {
		}

		/**
		 * Constructor.
		 *
		 * @param comparer Comparer used to determine the equality of items.
		 * @param tableLength Length of the table.
		 */
		Table(const EqualityComparer & comparer, size_t tableLength):
			storage(HashStorage(comparer, tableLength)) {
		}


		/**
		 * Inserts the {@code element} into the table.
		 *
		 * @param element Added element.
		 */
		void insert(const T & element) {
			this->storage.insert(element, this->function(element, this->storage.getTableSize()));
			if (this->rehashPolicy.needsRehashingAfterInsert(this->storage)) {
				this->rehash(Enlarge);
			}
		}

		/**
		 * Removes the {@code element} from the table.
		 *
		 * @param element Removed element.
		 */
		bool remove(const T & element) {
			bool removed = this->storage.remove(element, this->function(element, this->storage.getTableSize()));
			if (removed && !this->storage.isMinimal() && this->rehashPolicy.needsRehashingAfterDelete(this->storage)) {
				this->rehash(Shrink);
			}

			return removed;
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

		/**
		 * Completely rehashes the table.
		 */
		void rehash(RehashingOperation operation = Refresh) {
			// TODO: Resizing policy.
			size_t newLength = this->storage.getTableSize();
			switch (operation) {
				case Shrink:
					newLength /= 2;
					break;

				case Enlarge:
					newLength *= 2;
					break;

				case Refresh:
					break;

				default:
					// TODO: assert(false, "Should not get here.");
					break;
			}

			HashTable t(this->storage.getComparer(), newLength);

			for (HashTable::Iterator it = this->getBeginning(), ite = this->getEnd(); it != ite; ++it) {
				t.insert(*it);
			}

			swap(*this, t);
		}

		/**
		 * Beginning of the table - first item.
		 *
		 * @return Iterator pointing to the first item.
		 */
		Iterator getBeginning(void) {
			return this->storage.getBeginning();
		}

		/**
		 * End of the table - after the last item.
		 *
		 * @return Iterator pointing just after the last item.
		 */
		Iterator getEnd(void) {
			return this->storage.getEnd();
		}

		/**
		 * Beginning of the table - first item.
		 *
		 * @return Iterator pointing to the first item.
		 */
		const Iterator getBegin(void) const {
			return const_cast<HashTable *>(this)->getBegin();
		}

		/**
		 * End of the table - after the last item.
		 *
		 * @return Iterator pointing just after the last item.
		 */
		const Iterator getEnd(void) const {
			return const_cast<HashTable *>(this)->getEnd();
		}

		/**
		 * Swapping the hash tables.
		 *
		 * @param a Hash table to be swapped.
		 * @param b Hash table to be swapped.
		 */
		friend void swap(HashTable & a, HashTable & b) {
			// TODO: std::swap is bad.
			std::swap(a.function, b.function);
			swap(a.storage, b.storage);
			std::swap(a.rehashPolicy, b.rehashPolicy);
		}

		/**
		 * Hash function retrieval.
		 *
		 * @return Current hash function.
		 */
		const HashFunction & getFunction(void) const {
			return function;
		}

	private:
		/**
		 * Used hashed function.
		 */
		mutable HashFunction function;

		/**
		 * Used storage.
		 */
		HashStorage storage;

		/**
		 * Used policy for rehashing signalization.
		 */
		RehashPolicy rehashPolicy;

	};

}

#endif /* TABLE_H */
