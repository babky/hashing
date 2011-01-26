#ifndef COLLISION_COUNT_STORAGE_H
#define COLLISION_COUNT_STORAGE_H

#include "utils/chain_length_aware_storage_info.h"
#include "storage.h"
#include "utils/constant_comparer.h"

namespace Hash { namespace Storages {

	/** 
	 * Test storage which actually does not store the elements, it only counts the number of elements that would be 
	 * stored inside the chain.
	 *
	 * @typeparam T Type of the stored items. It is ignored, this storage does not store the data.
	 * @typeparam Comparer Comparer used do determine the key equality.  This parameters is ignored, too.
	 * @typeparam Hash Type of the hash value.
	 */
	template <typename T, typename Comparer, typename Hash> 
	class CollisionCountStorage : public Storage<T, Comparer, Hash, Utils::ChainLengthAwareStorageInfo> {
	public:
		/**
         * A single item of storage -- length of the chain.
		 */
		typedef size_t StorageItem;

		// Forward declaration for iterator.
		class CollisionCountStorageIterator;

		/**
		 * Iterator type for iterating chains.
		 */
		typedef CollisionCountStorageIterator Iterator;
	
		/**
		 * C-tor.
		 *
		 * @param comparer Default comparer used.
		 */
		explicit CollisionCountStorage(const Comparer & comparer, size_t tableLength = StorageParams::STARTING_STORAGE_SIZE):
		  size(0),
		  storageSize(tableLength),
		  storage(new StorageItem[tableLength]),
		  comparer(comparer) {
			  init();
		}		
		  
		/**
		 * C-tor.
		 *
		 * @param tableLength Default size of the table.
		 */
		explicit CollisionCountStorage(size_t tableLength = StorageParams::STARTING_STORAGE_SIZE):
		  size(0),
		  storageSize(tableLength),
		  storage(new StorageItem[tableLength]),
		  comparer(comparer) {
			  init();
		}

		virtual ~CollisionCountStorage(void) {
			delete [] storage;
			storage = 0;
		}

		virtual void insert(const T &, Hash hash) {
			++storage[hash];
			++size;
		}

		virtual bool remove(const T &, Hash) {
			return false;
		}

		virtual bool contains(const T &, Hash) const {
			return false;
		}

		virtual void clear(void) {
			for (size_t i = 0; i < storageSize; ++i) {
				storage[i] = 0;
			}
		}

		virtual Utils::ConstantComparer<T> getComparer(void) const {
			return comparer;
		}

		virtual bool isMinimal(void) const {
			return storageSize <= StorageParams::STARTING_STORAGE_SIZE;
		}

		virtual size_t getSize(void) const {
			return size;
		}

		virtual size_t getTableSize(void) const {
			return storageSize;
		}

		virtual double getLoadFactor(void) const {
			return 0;
		}

		virtual void computeStatistics(Utils::StorageStatistics & stats) const {
			stats.clear();
			for (size_t i = 0; i < storageSize; ++i) {
				stats.addChain(storage[i]);
			}
		}
		
		Iterator getBeginning(void) {
			return CollisionCountStorageIterator(this, true);
		}

		Iterator getEnd(void) {
			return CollisionCountStorageIterator(this, false);
		}

		/**
		 * Iterator for the table.
		 */
		class CollisionCountStorageIterator {
		public:
			CollisionCountStorageIterator(CollisionCountStorage * s, bool beginning):
			  storage(s),
			  tableIndex(beginning ? 0 : s->storageSize),
			  chainIndex(0) {
				// After initialization we should point at the valid (first) element.
				if (beginning && storage->storage[0] == 0) {
				  ++(*this);
				}
			}

			friend bool operator ==(const CollisionCountStorageIterator & a, const CollisionCountStorageIterator & b) {
				return a.storage == b.storage && a.tableIndex == b.tableIndex && a.chainIndex == b.chainIndex;
			}

			friend bool operator !=(const CollisionCountStorageIterator & a, const CollisionCountStorageIterator & b) {
				return !(a == b);
			}

			Hash operator *(void) const {
				return tableIndex;
			}

			CollisionCountStorageIterator operator ++(void) {
				// Move.
				++chainIndex;
				if (chainIndex >= storage->storage[tableIndex]) {
					// We are behind the last element, find the next chain.
					chainIndex = 0;
					while (tableIndex < storage->storageSize && storage->storage[tableIndex] == 0) {
						++tableIndex;
					}
				}

				return *this;
			}

			CollisionCountStorageIterator operator ++(int) {
				CollisionCountStorage tmp = *this;
				++(*this);
				return tmp;
			}

		private:
			/**
			 * Index of the iterated chain.
			 */
			size_t tableIndex;

			/**
			 * Index of the element in the chain.
			 */
			size_t chainIndex;

			/**
			 * Storage which is iterated.
			 */
			CollisionCountStorage * storage;
		};

		// ChainLengthAwareStorage
		virtual size_t getChainLength(size_t address) const {
			return storage[address];
		}

	protected:
		/**
		 * Initializes the table - sizes of all chains are zeroed.
		 */
		virtual void init(void) {
			// TODO: memset!
			for (size_t i = 0; i < storageSize; ++i) {
				storage[i] = 0;
			}
		}

		/**
		 * Sizes of the chains.
		 */
		StorageItem * storage;

		/**
		 * Size of the table.
		 */
		size_t storageSize;

		/**
         * The number of stored elements.
		 */
		size_t size;

		/**
		 * Comparer used.
		 */
		Utils::ConstantComparer<T> comparer;

	private:
		/**
		 * Swapping of the two storages.
		 *
		 * @param a Storage to be swapped.
		 * @param b Storage to be swapped.
		 */
		friend void swap(CollisionCountStorage<T, Comparer, Hash> & a, CollisionCountStorage<T, Comparer, Hash> & b) {
			std::swap(a.storage, b.storage);
			std::swap(a.storageSize, b.storageSize);
			std::swap(a.size, b.size);
			std::swap(a.comparer, b.comparer);
		}

	};

} }

#endif /* COLLISION_COUNT_STORAGE_H */