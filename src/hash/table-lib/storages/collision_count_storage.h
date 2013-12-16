#ifndef COLLISION_COUNT_STORAGE_H
#define COLLISION_COUNT_STORAGE_H

#include "storage.h"
#include "utils/constant_comparer.h"
#include "utils/hash_assert.h"
#include "utils/boost_include.h"
#include <boost/thread.hpp>

namespace Hash { namespace Storages {

	/**
	 * Test storage which actually does not store the elements, it only counts the number of elements that would be
	 * stored inside the chain.
	 *
	 * @typeparam T Type of the stored items. It is ignored, this storage does not store the data.
	 * @typeparam Comparer Comparer used do determine the key equality. This parameter is ignored, too.
	 * @typeparam Hash Type of the hash value.
	 * @typeparam ChainLengthType Length of the chain.
	 */
	template <typename T, typename Comparer, typename Hash>
	class CollisionCountStorage : public Storage<T, Comparer, Hash, PlainStorageInfo> {
	public:
		/**
		 * A single item of storage -- length of the chain.
		 */
		typedef boost::uint16_t StorageItem;

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
		 * @param tableSize Default table size.
		 */
		explicit CollisionCountStorage(const Comparer & comparer, size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE):
		  storageInfo(tableSize),
		  storage(new StorageItem[tableSize]),
		  comparer(comparer) {
			  init();
		}

		/**
		 * C-tor.
		 *
		 * @param tableSize Default size of the table.
		 */
		explicit CollisionCountStorage(size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE):
		  storageInfo(tableSize),
		  storage(new StorageItem[tableSize]),
		  comparer(comparer) {
			init();
		}

		/**
		 * Copy C-tor.
		 */
		CollisionCountStorage(const CollisionCountStorage & aStorage):
		  storageInfo(aStorage.storageInfo),
		  storage(new StorageItem[aStorage.storageInfo.getTableSize()]),
		  comparer(aStorage.comparer) {
			for (size_t i = 0, tableSize = aStorage.storageInfo.getTableSize(); i < tableSize; ++i) {
				this->storage[i] = aStorage.storage[i];
			}

			mutexes = new boost::shared_mutex[MUTEXES];
		}

		/**
		 * D-tor.
		 */
		~CollisionCountStorage(void) {
			delete [] storage;
			storage = 0;

			delete [] mutexes;
			mutexes = 0;
		}

		/**
		 * Assignment operator.
		 */
		CollisionCountStorage & operator=(const CollisionCountStorage & storage) {
			CollisionCountStorage tmp(storage);
			swap(tmp);
			return *this;
		}

		void insert(const T &, Hash hash) {
			// get upgradable access
			size_t storageSize = storageInfo.getTableSize();
			boost::upgrade_lock<boost::shared_mutex> l(mutexes[hash * MUTEXES / storageSize]);

			// get exclusive access
			boost::upgrade_to_unique_lock<boost::shared_mutex> uL(l);

			++storage[hash];
			storageInfo.incElementCount();
		}

		bool remove(const T &, Hash) {
			return false;
		}

		bool contains(const T &, Hash) const {
			return false;
		}

		void clear(void) {
			storageInfo.setElementCount(0);
			for (size_t i = 0, tableSize = storageInfo.getTableSize(); i < tableSize; ++i) {
				storage[i] = 0;
			}
		}

		Utils::ConstantComparer<T> getComparer(void) const {
			return comparer;
		}

		bool isMinimal(void) const {
			return storageInfo.getTableSize() <= StorageParams::INITIAL_STORAGE_SIZE;
		}

		size_t size(void) const {
			return storageInfo.getElementCount();
		}

		void computeStatistics(Utils::StorageStatistics & stats) const {
			stats.clear();
			for (size_t i = 0, tableSize = storageInfo.getTableSize(); i < tableSize; ++i) {
				stats.addChain(storage[i]);
			}
		}

		const PlainStorageInfo & getStorageInfo(void) const {
			return storageInfo;
		}

		Iterator getBeginning(void) {
			return CollisionCountStorageIterator(this, true);
		}

		Iterator getEnd(void) {
			return CollisionCountStorageIterator(this, false);
		}

		const static bool HAS_REHASH = false;

		template<class HashFunction>
		void rehash(CollisionCountStorage &, HashFunction & f) {
			simple_assert(false, "Collision count storage does not implement rehash.");
		}

		/**
		 * Iterator for the table.
		 */
		class CollisionCountStorageIterator {
		public:
			CollisionCountStorageIterator(CollisionCountStorage * s, bool beginning):
			  storage(s),
			  tableIndex(beginning ? 0 : s->storageInfo.getTableSize()),
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
				size_t tableSize = storage->storageInfo.getTableSize();

				// Move.
				++chainIndex;
				if (chainIndex >= storage->storage[tableIndex]) {
					// We are behind the last element, find the next chain.
					chainIndex = 0;
					for (++tableIndex; tableIndex < tableSize && storage->storage[tableIndex] == 0; ++tableIndex) {
					}
				}

				return *this;
			}

			CollisionCountStorageIterator operator ++(int) {
				CollisionCountStorageIterator tmp = *this;
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
		size_t getChainLength(size_t address) const {
			boost::shared_lock<boost::shared_mutex> lock(mutexes[address * MUTEXES / storageInfo.getTableSize()]);
			return storage[address];
		}

		/**
 		 * Swapping of the two storages.
		 *
		 * @param b Storage to be swapped.
		 */
		void swap(CollisionCountStorage<T, Comparer, Hash> & b) {
			std::swap(storage, b.storage);
			std::swap(storageInfo, b.storageInfo);
			std::swap(comparer, b.comparer);
			std::swap(mutexes, b.mutexes);
		}

		/**
		 * Swapping of the two storages.
		 *
		 * @param a Storage to be swapped.
		 * @param b Storage to be swapped.
		 */
		friend void swap(CollisionCountStorage<T, Comparer, Hash> & a, CollisionCountStorage<T, Comparer, Hash> & b) {
			a.swap(b);
		}

	protected:
		/**
		 * The number of mutexes when accessing the table.
		 */
		static const size_t MUTEXES = 4;

		/**
		 * Mutexes.
		 */
		mutable boost::shared_mutex * mutexes;

		/**
		 * Initializes the table - sizes of all chains are zeroed.
		 */
		virtual void init(void) {
			// TODO: memset!
			for (size_t i = 0, tableSize = storageInfo.getTableSize(); i < tableSize; ++i) {
				storage[i] = 0;
			}

			mutexes = new boost::shared_mutex[MUTEXES];
		}

	private:
		/**
		 * Sizes of the chains.
		 */
		StorageItem * storage;

		/**
		 * Current storage info.
		 */
		SettablePlainStorageInfo storageInfo;

		/**
		 * Comparer used.
		 */
		Utils::ConstantComparer<T> comparer;
	};

} }

namespace std {

	/**
	 * Swapping of the two storages.
	 *
	 * @param a Storage to be swapped.
	 * @param b Storage to be swapped.
	 */
	template <typename T, typename Comparer, typename HashType>
	void swap(Hash::Storages::CollisionCountStorage<T, Comparer, HashType> & a, Hash::Storages::CollisionCountStorage<T, Comparer, HashType> & b) {
		a.swap(b);
	}

}

#endif /* COLLISION_COUNT_STORAGE_H */
