#ifndef CHAINED_STORAGE_H
#define CHAINED_STORAGE_H

#include <algorithm>
#include "utils/chain_length_aware_storage_info.h"
#include "storage.h"

namespace Hash { namespace Storages {

	/**
	 * Probing storage for hash table.
	 *
	 * @typeparam T Type of the stored items.
	 * @typeparam Comparer Comparer used.
	 * @typeparam Hash Type of the hash.
	 */
	template <typename T, typename Comparer, typename Hash>
	class ProbingStorage : public Storage<T, Comparer, Hash, Utils::ChainLengthAwareStorageInfo> {
	public:
		/**
		 * Storage type used.
		 */
		typedef ProbingStorage<T, Comparer, Hash> Storage;

		/**
		 * Comparer used.
		 */
		typedef Comparer EqualityComparer;

		/**
		 * Type of the hash value.
		 */
		typedef Hash HashType;

		/**
		 * Iterator forward declaration.
		 */
		class ProbingStorageIterator;

		/**
		 * Iterator type definition.
		 */
		typedef ProbingStorageIterator Iterator;

		/**
		 * Proging storage c-tor.
		 */
		ProbingStorage(void):
		  elementCount(0),
		  storageLength(StorageParams::STARTING_STORAGE_SIZE),
		  storage(new StorageItem[StorageParams::STARTING_STORAGE_SIZE]),
		  comparer(EqualityComparer()) {
		}

		/**
		 * Probing storage c-tor.
		 *
		 * @param comparer Used comparer.
		 * @param tableLength Starting length of the table.
		 */
		explicit ProbingStorage(const EqualityComparer & comparer, size_t tableLength = StorageParams::STARTING_STORAGE_SIZE):
		  elementCount(0),
		  storageLength(tableLength),
		  storage(new StorageItem[tableLength]),
		  comparer(comparer) {
		}

		/**
		 * Probing storage d-tor.
		 */
		~ProbingStorage(void) {
			delete [] this->storage;
			this->storage = 0;
			this->storageLength = 0;
		}

		/**
		 * Chained storage copy c-tor.
		 *
		 * @param storage Copied storage.
		 */
		ProbingStorage(const ProbingStorage & storage) {
			this->storage = new StorageItem[storage.storageLength];
			this->storageLength = storage.storageLength;
			this->elementCount = storage.elementCount;
			this->comparer = storage.comparer;
		}

		/**
		 * Probing storage assignment operator.
		 *
		 * @param storage Copied storage.
		 */
		ProbingStorage & operator =(const ProbingStorage & storage) {
			ProbingStorage tmp = storage;
			swap(tmp);
			return *this;
		}

		void insert(const T & item, HashType hash) {
			simple_assert(hash < storageLength, "Hash must be inside the storage!");

			if (!this->storage[hash].empty) {
				throw BucketOccupied<HashType>(hash);
			}

			this->storage[hash].empty = false;
			this->storage[hash].item = item;
			++this->elementCount;
		}

		bool remove(const T & item, HashType hash) {
			simple_assert(hash < storageLength, "Hash must be inside the storage!");

			if (this->storage[hash].empty) {
				return false;
			} else if (this->comparer(item, this->storage[hash])) {
				this->storage[hash].empty = true;
				--this->elementCount;
			} else {
				return false;
			}
		}

		bool contains(const T & item, HashType hash) const {
			return !this->storage[hash].empty && this->comparer(item, this->storage[hash].item);
		}

		size_t getSize(void) const {
			return this->elementCount;
		}

		size_t getTableSize(void) const {
			return this->storageLength;
		}

		void clear(void) {
			this->storageLength = 0;
			this->elementCount = 0;
			delete [] this->storage;
			this->storage = new StorageItem[StorageParams::STARTING_STORAGE_SIZE];
			this->storageLength = StorageParams::STARTING_STORAGE_SIZE;
		}

		double getLoadFactor(void) const {
			simple_assert(storageLength != 0, "Storage must contain at least one slot.");
			return static_cast<double>(this->elementCount) / this->storageLength;
		}

		void computeStatistics(Utils::StorageStatistics & stats) const {
			// TODO: Improve the StorageStatistics class so that it can handle probing storage properly.
			stats.clear();
		}

		bool isMinimal(void) const {
			return this->getTableSize() <= StorageParams::STARTING_STORAGE_SIZE;
		}

		Comparer getComparer(void) const {
			return this->comparer;
		}

		Iterator getBeginning(void) {
			return ProbingStorageIterator(this, true);
		}

		Iterator getEnd(void) {
			return ProbingStorageIterator(this, false);
		}

	private:
		/**
		 * Single item for representing a stored element.
		 */
		typedef struct {
			/**
			 * Flag indicating if the item is empty.
			 */
			bool empty;

			/**
			 * Represented element.
			 */
			T item;
		} StorageItem;

		/**
		 * Number of items in the table.
		 */
		size_t elementCount;

		/**
		 * Table of chains.
		 */
		StorageItem * storage;

		/**
		 * Length of the table.
		 */
		size_t storageLength;

		/**
		 * Comparer.
		 */
		mutable EqualityComparer comparer;

	public:
		class ProbingStorageIterator {
		public:
			explicit ProbingStorageIterator(ProbingStorage * aStorage, HashType aPosition = 0):
			  storage(aStorage),
			  position(aPosition)
			{
			}

		protected:
			ProbingStorage * storage;
			HashType position;
		};
		
		/**
		 * Swapping of the two storages.
		 *
		 * @param a Storage to be swapped.
		 * @param b Storage to be swapped.
		 */
		void swap(ChainedStorage<T, EqualityComparer, HashType> & b) {
			using std::swap;

			swap(storage, b.storage);
			swap(comparer, b.comparer);
			swap(elementCount, b.elementCount);
			swap(storageLength, b.storageLength);
		}

		friend void swap(ChainedStorage & a, ChainedStorage & b) {
			a.swap(b);
		}

	};

} }

namespace std {

	template <typename T, typename Comparer, typename HashType>
	void swap(Hash::Storages::ChainedStorage<T, Comparer, HashType> & a, Hash::Storages::ChainedStorage<T, Comparer, HashType> & b) {
		a.swap(b);
	}

}

#endif /* CHAINED_STORAGE_H */
