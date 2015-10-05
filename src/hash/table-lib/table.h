#ifndef TABLE_H
#define TABLE_H

#include <algorithm>
#include <iostream>
#include "utils/definitions.h"
#include "utils/smart_pointer.h"
#include "utils/storage_statistics.h"
#include "policies/rehash_policy.h"
#include <stdexcept>
#include <cassert>

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
		template <class, class> class Function,
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
		 * Type for the hash. This is the type of the value returned by the hash function.
		 */
		typedef size_t HashType;

		/**
		 * Equality comparer.
		 */
		typedef Comparer EqualityComparer;

		/**
		 * Storage - hash table implementation.
		 */
		typedef Storage<T, EqualityComparer, HashType> HashStorage;

		/**
		 * Type for the hash function.
		 */
		typedef Function<T, HashStorage> HashFunction;

		/**
		 * Iterator for iterating throughout the items of the table.
		 */
		typedef typename HashStorage::Iterator Iterator;

	private:
		/**
		 * Table's initializer.
		 */
		void init(void) {
			this->function.setTableSize(this->storage.getStorageInfo().getTableSize());
			this->function.reset();
			this->function.setStorage(&storage);
		}

	public:
		/**
		 * Constructor.
		 */
		Table(void) {
			init();
		}

		/**
		 * Constructor.
		 *
		 * @param tableSize Size of the table.
		 */
		explicit Table(size_t tableSize):
		  storage(HashStorage(tableSize)) {
			init();
		}

		/**
		 * Constructor.
		 *
		 * @param comparer Comparer used to determine the equality of items.
		 */
		explicit Table(const EqualityComparer & comparer):
		  storage(HashStorage(comparer)) {
			init();
		}

		/**
		 * Constructor.
		 *
		 * @param comparer Comparer used to determine the equality of items.
		 * @param tableSize Size of the table.
		 */
		Table(const EqualityComparer & comparer, size_t tableSize):
		  storage(HashStorage(comparer, tableSize)) {
			init();
		}

		/**
		 * Inserts the {@code element} into the table.
		 *
		 * @param element Added element.
		 */
		void insert(const T & element) {
			storage.insert(element, function(element));
			if (rehashPolicy.needsRehashingAfterInsert(storage.getStorageInfo())) {
				rehash(Enlarge);
			}
		}

		/**
		 * Removes the {@code element} from the table.
		 *
		 * @param element Removed element.
		 */
		bool remove(const T & element) {
			bool removed = storage.remove(element, function(element));
			if (removed && !storage.isMinimal() && rehashPolicy.needsRehashingAfterDelete(storage.getStorageInfo())) {
				 rehash(Shrink);
			}

			return removed;
		}

		/**
		 * Number of stored elements retrieval.
		 *
		 * @return Number of hashed elements.
		 */
		size_t size(void) const {
			return storage.size();
		}

		/**
		 * The size of the table retrieval.
		 *
		 * @return Size of the table.
		 */
		size_t getTableSize(void) const {
			return storage.getStorageInfo().getTableSize();
		}

		/**
		 * Containment check.
		 *
		 * @param element Element to be checked for.
		 * @return Containment check.
		 */
		bool contains(const T & element) const {
			return storage.contains(element, function(element));
		}

		/**
		 * Clears the stored set.
		 */
		void clear(void) {
			storage.clear();
			function.setTableSize(storage.getStorageInfo().getTableSize());
			function.reset();
		}

		/**
		 * Load factor retrieval.
		 *
		 * @return Load factor.
		 */
		double getLoadFactor(void) const {
			return storage.getStorageInfo().getLoadFactor();
		}

		/**
		 * Computes the statistics for this storage.
		 *
		 * @param stats Storage for statistics.
		 */
		void computeStatistics(Utils::StorageStatistics & stats) const {
			storage.computeStatistics(stats);
		}

		/**
		 * Completely rehashes the table.
		 */
		void rehash(RehashingOperation operation = Refresh) {
			// TODO: Resizing policy.
			size_t newSize = storage.getStorageInfo().getTableSize();
			switch (operation) {
				case Shrink:
					newSize /= 2;
					break;

				case Enlarge:
					newSize *= 2;
					break;

				case Refresh:
					break;

				default:
					assert(false);
					break;
			}

			rehash(newSize);
		}

	private:
		void rehash(size_t newSize) {
			if (HashStorage::HAS_REHASH) {
				HashStorage tmp(storage.getComparer(), newSize);
				function.setTableSize(newSize);
				storage.rehash(tmp, function);
				storage.swap(tmp);
			} else {
				HashTable t(storage.getComparer(), newSize);
				for (HashTable::Iterator it = getBeginning(), ite = getEnd(); it != ite; ++it) {
					t.insert(*it);
				}
				swap(t);
			}
		}

	public:
		/**
		 * Beginning of the table - first item.
		 *
		 * @return Iterator pointing to the first item.
		 */
		Iterator getBeginning(void) {
			return storage.getBeginning();
		}

		/**
		 * End of the table - after the last item.
		 *
		 * @return Iterator pointing just after the last item.
		 */
		Iterator getEnd(void) {
			return storage.getEnd();
		}

		/**
		 * Beginning of the table - first item.
		 *
		 * @return Iterator pointing to the first item.
		 */
		const Iterator getBeginning(void) const {
			return const_cast<HashTable *>(this)->getBeginning();
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
		 * @param b Hash table to be swapped.
		 */
		void swap(HashTable & b) {
			storage.swap(b.storage);
			function.swap(b.function);
			std::swap(rehashPolicy, b.rehashPolicy);
		}

		/**
		 * Swapping the hash tables.
		 *
		 * @param a Hash table to be swapped.
		 * @param b Hash table to be swapped.
		 */
		friend void swap(HashTable & a, HashTable & b) {
			a.swap(b);
		}

		/**
		 * Hash function retrieval.
		 *
		 * @return Current hash function.
		 */
		HashFunction & getFunction(void) {
			return function;
		}

		/**
		 * Hash function retrieval.
		 *
		 * @return Current hash function.
		 */
		const HashFunction & getFunction(void) const {
			return const_cast<Table<T, Comparer, Function, Storage, RehashPolicy> *> (this)->getFunction();
		}

		/**
		 * Rehash policy retrieval.
		 *
		 * @return Rehash policy.
		 */
		RehashPolicy & getRehashPolicy(void) {
			return getRehashPolicy();
		}

		/**
		 * Rehash policy retrieval.
		 *
		 * @return Rehash policy.
		 */
		const RehashPolicy & getRehashPolicy(void) const {
			const_cast<Table<T, Comparer, Function, Storage, RehashPolicy> *> (this)->getRehashPolicy();
		}

		/**
		 * Rehash policy setter.
		 *
		 * @param rehashPolicy New rehash policy.
		 */
		void setRehashPolicy(const RehashPolicy & rehashPolicy) {
			this->rehashPolicy = rehashPolicy;
		}

		/**
		 * Reserve the size.
		 *
		 * @param reservedSize New reserved size.
		 */
		void reserve(std::size_t reservedSize) {
			rehash(reservedSize);
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
