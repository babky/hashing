#ifndef DIRECT_CHAINING_STORAGE_H
#define DIRECT_CHAINING_STORAGE_H

#include <algorithm>
#include <utility>
#include "utils/boost_include.h"
#include "storage.h"
#include "utils/hash_assert.h"
#include <iostream>

namespace Hash { namespace Storages {

	/**
	 * Separate chaining storage for a hash table. Chains are represented directly in the hash table.
	 *
	 * @typeparam T Type of the stored items.
	 * @typeparam Comparer Comparer used.
	 * @typeparam Hash Type of the hash.
	 */
	template <typename T, typename Comparer, typename Hash>
	class DirectChainingStorage : public Storage<T, Comparer, Hash, SettablePlainStorageInfo> {
	public:
		/**
		 * Comparer used.
		 */
		typedef Comparer EqualityComparer;

		/**
		 * Type of the hash value.
		 */
		typedef Hash HashType;

		// Forward declaration for iterator.
		class ChainingStorageIterator;

		/**
		 * Iterator for the storage.
		 */
		typedef ChainingStorageIterator Iterator;

		typedef SettablePlainStorageInfo StorageInfo;

		/**
		 * Storage c-tor.
		 *
		 * @param tableSize Starting length of the table.
		 */
		explicit DirectChainingStorage(size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE):
		  storageInfo(tableSize),
		  storage(new StorageItem[tableSize]),
		  comparer(EqualityComparer()) {
		}

		/**
		 * Storage c-tor.
		 *
		 * @param comparer Used comparer.
		 * @param tableSize Starting length of the table.
		 */
		explicit DirectChainingStorage(const EqualityComparer & comparer, 
			size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE):
		  storageInfo(tableSize),
		  storage(new StorageItem[tableSize]),
		  comparer(comparer) {
		}

		/**
		 * Storage d-tor.
		 */
		~DirectChainingStorage(void) {
			delete [] storage;
			storage = 0;
		}

		/**
		 * Storage copy c-tor.
		 *
		 * @param storage Copied storage.
		 */
		DirectChainingStorage(const DirectChainingStorage & aStorage):
		  storageInfo(aStorage.storageInfo),
		  storage(new StorageItem[aStorage.storageInfo.getTableSize()]),
		  comparer(aStorage.comparer) {
			for (size_t i = 0, e = aStorage.storageInfo.getTableSize(); i < e; ++i) {
				storage[i] = aStorage.storage[i];
			}
		}

		/**
		 * Storage assignment operator.
		 *
		 * @param storage Copied storage.
		 */
		DirectChainingStorage & operator =(const DirectChainingStorage & storage) {
			DirectChainingStorage tmp = storage;
			swap(tmp);
			return *this;
		}

		void insert(const T & item, HashType hash) {
			size_t tableSize = storageInfo.getTableSize();
			size_t pos = EMPTY_ROW;
			simple_assert(hash < tableSize, "Hash must be inside the storage!");

			if (storage[hash].start == EMPTY_ROW) {
				// Creating a new chain.
				if (storage[hash].next == EMPTY_ROW) {
					// No one is pointing here, so we can put it here.
					storage[hash].start = hash;
					storage[hash].item = item;
					storage[hash].next = LAST_CHAIN_ELEMENT;
				} else {
					// Find a new row.
					for (pos = (hash + 1) % tableSize; storage[pos].next != EMPTY_ROW; pos = (pos + 1) % tableSize) {
					}

					storage[hash].start = pos;
					storage[pos].next = LAST_CHAIN_ELEMENT;
					storage[pos].item = item;
				}
			} else {
				// Put the new item to be last in the chain.

				// Find the last element.
				for (pos = storage[hash].start; storage[pos].next != LAST_CHAIN_ELEMENT; pos = storage[pos].next) {
					if (comparer(storage[pos].item, item)) {
						throw ItemStoredException<T>(item);
					}
				}

				if (comparer(storage[pos].item, item)) {
					throw ItemStoredException<T>(item);
				}

				// Find the new row.
				size_t newPos = (pos + 1) % tableSize;
				for (; storage[newPos].next != EMPTY_ROW; newPos = (newPos + 1) % tableSize) {
				}

				// Remember it there.
				storage[pos].next = newPos;
				storage[newPos].next = LAST_CHAIN_ELEMENT;
				storage[newPos].item = item;
			}

			storageInfo.incElementCount();
		}

		bool remove(const T & item, HashType hash) {
			std::pair<size_t, size_t> findResult = findPos(item, hash);
			size_t pos = findResult.first;
			size_t prev = findResult.second;
			if (pos == EMPTY_ROW) {
				// Found nothing.
				return false;
			}

			if (prev == EMPTY_ROW) {
				// The first element in the chain.
				storage[hash].start = storage[pos].next;

				// If the element was last in the chain, then set the flags properly.
				if (storage[hash].start == LAST_CHAIN_ELEMENT) {
					storage[hash].start = EMPTY_ROW;
				}
			} else {
				// Skip pos.
				storage[prev].next = storage[pos].next;
			}
			// Mark the position pos as empty.
			storage[pos].next = EMPTY_ROW;

			// Success.
			storageInfo.decElementCount();
			return true;
		}

		bool contains(const T & item, HashType hash) const {
			return findPos(item, hash).first != EMPTY_ROW;
		}

private:
		std::pair<size_t, size_t> findPos(const T & item, HashType hash) const {
			size_t emptyRow = EMPTY_ROW;
			size_t prev = emptyRow;
			for (size_t pos = storage[hash].start; pos < LAST_CHAIN_ELEMENT; prev = pos, pos = storage[pos].next) {
				if (comparer(storage[pos].item, item)) {
					return std::make_pair(pos, prev);
				}
			}

			return std::make_pair(emptyRow, prev);
		}

public:
		size_t size(void) const {
			return storageInfo.getElementCount();
		}
		
		void clear(void) {
			for (size_t i = 0; i < storageInfo.getTableSize(); ++i) {
				storage[i] = StorageItem();
			}
			storageInfo.setElementCount(0);
		}

		const StorageInfo & getStorageInfo(void) const {
			return storageInfo;
		}

		void computeStatistics(Utils::StorageStatistics & stats) const {
			size_t chainSize = 0;
			for (size_t i = 0; i < storageInfo.getTableSize(); ++i) {
				chainSize = 0;

				if (storage[i].start != EMPTY_ROW) {
					for (size_t j = storage[i].start; j < LAST_CHAIN_ELEMENT; j = storage[j].next) {
						++chainSize;
					}
				}

				stats.addChain(chainSize);
			}
		}

		bool isMinimal(void) const {
			return storageInfo.getTableSize() <= StorageParams::INITIAL_STORAGE_SIZE;
		}

		Comparer getComparer(void) const {
			return comparer;
		}

		Iterator getBeginning(void) {
			return ChainingStorageIterator(this, 0);
		}

		Iterator getEnd(void) {
			return ChainingStorageIterator(this, storageInfo.getTableSize());
		}

		static const bool HAS_REHASH = true;

		template<class Function>
		void rehash(DirectChainingStorage & s, Function & f) {
			for (size_t i = 0, e = storageInfo.getTableSize(); i < e; ++i) {
				if (storage[i].next == EMPTY_ROW) {
					continue;
				}

				s.insert(storage[i].item, f(storage[i].item));
			}
		}

		/**
		 * Swapping of the two storages.
		 *
		 * @param b Storage to be swapped with.
		 */
		void swap(DirectChainingStorage & b) {
			using std::swap;

			swap(storage, b.storage);
			swap(comparer, b.comparer);
			swap(storageInfo, b.storageInfo);
		}

		/**
		 * Swapping of the two storages.
		 *
		 * @param a Storage to be swapped.
		 * @param b Storage to be swapped.
		 */
		friend void swap(DirectChainingStorage & a, DirectChainingStorage & b) {
			a.swap(b);
		}

	public:
		class ChainingStorageIterator {
		public:
			explicit ChainingStorageIterator(DirectChainingStorage * aStorage, size_t startPos = 0):
				storage(aStorage),
				pos(startPos)
			{
				if (storage->storage[pos].next == EMPTY_ROW) {
					++(*this);
				}
			}

			friend bool operator==(const ChainingStorageIterator & a, const ChainingStorageIterator & b) {
				return a.storage + a.pos == b.storage + b.pos;
			}

			friend bool operator!=(const ChainingStorageIterator & a, const ChainingStorageIterator & b) {
				return !(a == b);
			}

			T & operator *(void) {
				simple_assert(storage->storage[pos].next != EMPTY_ROW, "Can not use EMPTY_ROW as next with value.");
				return storage->storage[pos].item;
			}

			const T & operator *(void) const {
				return const_cast<ChainingStorageIterator *> (this)->operator *();
			}

			ChainingStorageIterator & operator++(void) {
				size_t tableSize = storage->storageInfo.getTableSize();
				if (pos == tableSize) {
					return *this;
				}

				for (++pos; storage->storage[pos].next == EMPTY_ROW && pos < tableSize; ++pos) {
				}

				return *this;
			}

			ChainingStorageIterator operator++(int) {
				ChainingStorageIterator tmp = *this;
				++(*this);
				return tmp;
			}

		private:
			DirectChainingStorage * storage;
			size_t pos;
		};

	protected:
		static const size_t EMPTY_ROW = boost::integer_traits<size_t>::const_max;
		static const size_t LAST_CHAIN_ELEMENT = boost::integer_traits<size_t>::const_max - 1;

		struct StorageItem {
			StorageItem(void):
				next(EMPTY_ROW),
				start(EMPTY_ROW)
			{
			}

			size_t next;
			size_t start;
			T item;
		};

		/**
		 * The storage info.
		 */
		StorageInfo storageInfo;

		/**
		 * Table of chains.
		 */
		StorageItem * storage;

		/**
		 * Comparer.
		 */
		mutable EqualityComparer comparer;

	};

} }

namespace std {

	template <typename T, typename Comparer, typename HashType>
	void swap(Hash::Storages::DirectChainingStorage<T, Comparer, HashType> & a, Hash::Storages::DirectChainingStorage<T, Comparer, HashType> & b) {
		a.swap(b);
	}

}

#endif /* CHAINED_STORAGE_H */
