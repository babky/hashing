#ifndef CHAINED_STORAGE_H
#define CHAINED_STORAGE_H

#include <algorithm>
#include "storage.h"
#include "../utils/hash_assert.h"

namespace Hash { namespace Storages {

	/**
	 * Separate chaining storage for a hash table. Chains are represented by linked lists.
	 *
	 * @typeparam T Type of the stored items.
	 * @typeparam Comparer Comparer used.
	 * @typeparam Hash Type of the hash.
	 * @typeparam SettableStorageInfo Storage info - allows the setters.
	 */
	template <typename T, typename Comparer, typename Hash, class SettableStorageInfo>
	class ChainedStorageBase : public Storage<T, Comparer, Hash, typename SettableStorageInfo::StorageInfo> {
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
		class ChainedStorageIterator;

		/**
		 * Iterator for the storage.
		 */
		typedef ChainedStorageIterator Iterator;

		typedef typename SettableStorageInfo::StorageInfo StorageInfo;

		/**
		 * Chained storage c-tor.
		 *
		 * @param tableSize Starting length of the table.
		 */
		explicit ChainedStorageBase(size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE):
		  storageInfo(tableSize),
		  storage(new ChainedList[tableSize]),
		  comparer(EqualityComparer()) {
		}

		/**
		 * Chained storage c-tor.
		 *
		 * @param comparer Used comparer.
		 * @param tableSize Starting length of the table.
		 */
		explicit ChainedStorageBase(const EqualityComparer & comparer, 
			size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE):
		  storageInfo(tableSize),
		  storage(new ChainedList[tableSize]),
		  comparer(comparer) {
		}

		/**
		 * Chained storage d-tor.
		 */
		~ChainedStorageBase(void) {
			delete [] storage;
			storage = 0;
		}

		/**
		 * Chained storage copy c-tor.
		 *
		 * @param storage Copied storage.
		 */
		ChainedStorageBase(const ChainedStorageBase & aStorage):
		  storageInfo(aStorage.storageInfo),
		  storage(new ChainedList[aStorage.storageInfo.getTableSize()]),
		  comparer(aStorage.comparer) {
			for (size_t i = 0, e = storageInfo.getTableSize(); i < e; ++i) {
				storage[i] = aStorage.storage[i];
			}
		}

		/**
		 * Chained storage assignment operator.
		 *
		 * @param storage Copied storage.
		 */
		virtual ChainedStorageBase & operator =(const ChainedStorageBase & storage) {
			ChainedStorageBase tmp = storage;
			swap(tmp);
			return *this;
		}

		void insert(const T & item, HashType hash) {
			simple_assert(hash < storageInfo.getTableSize(), "Hash must be inside the storage!");

			if (!storage[hash].insert(item, comparer)) {
				throw ItemStoredException<T>(item);
			}

			storageInfo.incElementCount();
		}

		bool remove(const T & item, HashType hash) {
			simple_assert(hash < storageInfo.getTableSize(), "Hash must be inside the storage!");

			if (storage[hash].remove(item, comparer)) {
				storageInfo.decElementCount();
				return true;
			} else {
				return false;
			}
		}

		bool contains(const T & item, HashType hash) const {
			return storage[hash].contains(item, comparer);
		}

		size_t size(void) const {
			return storageInfo.getElementCount();
		}

		void clear(void) {
			delete [] storage;
			storage = new ChainedList[StorageParams::INITIAL_STORAGE_SIZE];
			storageInfo.setTableSize(StorageParams::INITIAL_STORAGE_SIZE);
			storageInfo.setElementCount(0);
		}

		const StorageInfo & getStorageInfo(void) const {
			return storageInfo;
		}

		void computeStatistics(Utils::StorageStatistics & stats) const {
			for (size_t i = 0; i < storageInfo.getTableSize(); ++i) {
				stats.addChain(storage[i].getSize());
			}
		}

		bool isMinimal(void) const {
			return storageInfo.getTableSize() <= StorageParams::INITIAL_STORAGE_SIZE;
		}

		Comparer getComparer(void) const {
			return comparer;
		}

		Iterator getBeginning(void) {
			return ChainedStorageIterator(this, true);
		}

		Iterator getEnd(void) {
			return ChainedStorageIterator(this, false);
		}

		size_t getChainLength(HashType address) const {
			return storage[address].getSize();
		}

		static const bool HAS_REHASH = false;

		template<class Function>
		void rehash(ChainedStorageBase &, Function & f) {
			simple_assert(false, "Chained storage can not rehash.");
		}

		/**
		 * Swapping of the two storages.
		 *
		 * @param a Storage to be swapped.
		 * @param b Storage to be swapped.
		 */
		void swap(ChainedStorageBase & b) {
			using std::swap;

			swap(storage, b.storage);
			swap(comparer, b.comparer);
			swap(storageInfo, b.storageInfo);
		}

		friend void swap(ChainedStorageBase & a, ChainedStorageBase & b) {
			a.swap(b);
		}

	private:
		/**
		 * Node of the chain of items with the same hash code.
		 */
		struct ChainedNode {

			/**
			 * Swaps the contents of the given nodes.
			 *
			 * @param b Node to be swapped.
			 * @param a Node to be swapped.
			 */
			friend void swap(ChainedNode & a, ChainedNode & b) {
				std::swap(a.item, b.item);
				std::swap(a.next, b.next);
			}

			/**
			 * One node in the chain creation.
			 *
			 * @param item Item stored inside the chain node.
			 * @param next Next node in the chain.
			 */
			explicit ChainedNode(const T & aItem, ChainedNode * aNext = 0):
			  item(aItem),
			  next(aNext) {
			}

			/**
			 * Destructs the node.
			 * Also destructs the next node if it exists.
			 */
			~ChainedNode(void) {
				// TODO: Decide if this is the right behaviour.
				delete next;
				next = 0;
			}

			/**
			 * Item stored inside this node.
			 */
			T item;

			/**
			 * Next node.
			 */
			ChainedNode * next;

		private:
			/**
			 * Forbidden copy c-tor.
			 */
			ChainedNode(const ChainedNode &);

			/**
			 * Forbidden assignment operator.
			 */
			ChainedNode & operator =(const ChainedNode &);
		};

		/**
		 * Iterator responsible for iterating through a single chain.
		 */
		class ChainIterator {
		public:
			explicit ChainIterator(ChainedNode ** nodePtr) {
				this->nodePtr = nodePtr;
			}

			ChainIterator & operator ++(void) {
				if (*nodePtr == 0) {
					return *this;
				}

				nodePtr = &((*nodePtr)->next);

				return *this;
			}

			ChainIterator operator ++(int) {
				ChainIterator tmp = *this;
				++this;
				return tmp;
			}

			friend bool operator ==(const ChainIterator & a, const ChainIterator & b) {
				return (a.nodePtr == b.nodePtr) ||
						(b.nodePtr != 0 && a.nodePtr == 0 && *(b.nodePtr) == 0) ||
						(a.nodePtr != 0 && b.nodePtr == 0 && *(a.nodePtr) == 0);
			}

			friend bool operator !=(const ChainIterator & a, const ChainIterator & b) {
				return !(a == b);
			}

			T & operator *(void) {
				return (*nodePtr)->item;
			}

		private:
			ChainedNode ** nodePtr;
		};

		/**
		 * List of items stored inside a chain.
		 */
		class ChainedList {
			/**
			 * Swaps the lists.
			 *
			 * @param a Swapped list.
			 * @param b Swapped list.
			 */
			friend void swap(ChainedList & a, ChainedList & b) {
				a.swap(b);
			}

		public:
			/**
			 * Empty list creation.
			 */
			ChainedList(void):
			  first(0),
			  elementCount(0) {
			}

			/**
			 * List with the first element.
			 *
			 * @param element First item of the list.
			 */
			explicit ChainedList(const T & element):
			  first(new ChainedNode(element)), 
			  elementCount(1) {
			}

			/**
			 * Destruction of the whole list. Destructs all the nodes.
			 */
			~ChainedList(void) {
				ChainedNode * current = 0;
				for (ChainedNode * n = this->first; n != 0;) {
					current = n;
					n = n->next;

					// Removes the node the following assignment makes it not to delete its successor.
					current->next = 0;
					delete current;
				}

				this->first = 0;
				this->elementCount = 0;
			}

			/**
			 * Copy of the list.
			 *
			 * @param list Copied list.
			 */
			ChainedList(const ChainedList & list):
			  elementCount(list.elementCount),
			  first(0) {
				if (list.first) {
					for (ChainedNode * n = list.first; n->next; n = n->next) {
						first = new ChainedNode(n->item, first);
					}
				}
			}

			/**
			 * Assignment operator.
			 */
			ChainedList & operator =(const ChainedList & list) {
				ChainedList tmp = list;
				this->swap(tmp);
				return *this;
			}

			/**
			 * Inclusion test.
			 *
			 * @param item Item to be checked if it is inside the chain.
			 * @param comparer Comparer to be used when comparing the items inside the chain.
			 *
			 * @return True iff the item is stored inside the chain.
			 */
			bool contains(const T & item, EqualityComparer & comparer) const {
				for (ChainedNode * n = this->first; n != 0; n = n->next) {
					// TODO: assert(hash of item == hash of n->item);
					if (comparer(n->item, item)) {
						return true;
					}
				}

				return false;
			}

			/**
			 * Appends the item.
			 *
			 * @param item Item to be inserted if it is not inside the chain.
			 * @param comparer Comparer to be used when comparing the items inside the chain.
			 *
			 * @return Insertion status, {@code false} iff the item has already been included in the chain.
			 */
			bool insert(const T & item, EqualityComparer & comparer) {
				ChainedNode ** n = 0;
				for (n = &(this->first); (*n) != 0; n = &((*n)->next)) {
					if (comparer((*n)->item, item)) {
						return false;
					}
				}

				*n = new ChainedNode(item);
				simple_assert(*n != 0, "Newly created item may not be null.");
				++this->elementCount;
				return true;
			}

			/**
			 * Removes the item.
			 *
			 * @param item Item to be removed if it is inside the chain.
			 * @param comparer Comparer to be used when comparing the items inside the chain.
			 */
			bool remove(const T & item, EqualityComparer & comparer) {
				for (ChainedNode ** n = &(this->first); (*n) != 0; n = &((*n)->next)) {
					if (comparer((*n)->item, item)) {
						// Remember the node.
						ChainedNode * c = (*n);

						// Move to the next one.
						*n = (*n)->next;

						// So that we will not delete the rest of the list.
						c->next = 0;
						delete c;

						--this->elementCount;
						return true;
					}
				}

				return false;
			}

			/**
			 * Beginning of the chain retrieval.
			 */
			ChainIterator getBeginning(void) {
				return ChainIterator(&this->first);
			}

			/**
			 * End of the chain retrieval.
			 */
			ChainIterator getEnd(void) {
				return ChainIterator(0);
			}

			/**
			 * Number of elements stored inside this chain.
			 *
			 * @return Number of stored nodes.
			 */
			size_t getSize(void) const {
				return this->elementCount;
			};

			/**
			 * Swaps the current list and the list b.
			 *
			 * @param b List to be switched with.
			 */
			void swap(ChainedList & b) {
				std::swap(first, b.first);
				std::swap(elementCount, b.elementCount);
			}

		private:
			/**
			 * Number of elements stored inside this chain.
			 */
			size_t elementCount;

			/**
			 * First node of this chain.
			 */
			ChainedNode * first;
		};

	public:
		class ChainedStorageIterator {
		public:
			ChainedStorageIterator(const ChainedStorageBase * storage, bool beginning):
			  storage(storage),
			  chainIndex(beginning ? 0 : storage->storageInfo.getTableSize()),
			  chainIterator(beginning ? storage->storage[chainIndex].getBeginning() : ChainIterator(0)) {

				if (this->chainIterator == this->storage->storage[this->chainIndex].getEnd() && beginning) {
					++(*this);
				}
			}

			friend bool operator ==(const ChainedStorageIterator & a, const ChainedStorageIterator & b) {
				return a.storage == b.storage && a.chainIndex == b.chainIndex && b.chainIterator == a.chainIterator;
			}

			friend bool operator !=(const ChainedStorageIterator & a, const ChainedStorageIterator & b) {
				return !(a == b);
			}

			T & operator *(void) {
				return *(this->chainIterator);
			}

			const T & operator *(void) const {
				return *(this->chainIterator);
			}

			ChainedStorageIterator & operator++(void) {
				++this->chainIterator;
				size_t tableSize = this->storage->storageInfo.getTableSize();
				while (this->chainIterator == this->storage->storage[this->chainIndex].getEnd()) {
					++this->chainIndex;
					if (this->chainIndex == tableSize) {
						this->chainIterator = ChainIterator(0);
						break;
					}

					this->chainIterator = this->storage->storage[this->chainIndex].getBeginning();
				}

				return *this;
			}

			ChainedStorageIterator operator++(int) {
				ChainedStorageIterator tmp = *this;
				++(*this);
				return tmp;
			}

		private:
			const ChainedStorageBase * storage;
			size_t chainIndex;
			ChainIterator chainIterator;
		};

	protected:
		/**
		 * The storage info.
		 */
		SettableStorageInfo storageInfo;

	private:		
		/**
		 * Table of chains.
		 */
		ChainedList * storage;

		/**
		 * Comparer.
		 */
		mutable EqualityComparer comparer;

	};

	template<typename T, typename Comparer, typename HashType>
	class ChainedStorage : public ChainedStorageBase<T, Comparer, HashType, SettablePlainStorageInfo> {
	public:
		typedef Comparer EqualityComparer;
		typedef ChainedStorageBase<T, Comparer, HashType, SettablePlainStorageInfo> BaseStorage;

		/**
		 * Chained storage c-tor.
		 *
		 * @param tableSize Starting length of the table.
		 */
		explicit ChainedStorage(size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE):
		  BaseStorage(tableSize)
		{
		}

		/**
		 * Chained storage c-tor.
		 *
		 * @param comparer Used comparer.
		 * @param tableSize Starting length of the table.
		 */
		explicit ChainedStorage(const EqualityComparer & comparer, 
			size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE):
		  BaseStorage(comparer, tableSize)
		{
		}
		  
		/**
		 * Chained storage copy c-tor.
		 *
		 * @param storage Copied storage.
		 */
		ChainedStorage(const ChainedStorage & storage): 
		  BaseStorage(storage)
		{
		}

		/**
		 * Chained storage assignment operator.
		 *
		 * @param storage Copied storage.
		 */
		virtual ChainedStorage & operator =(const ChainedStorage & storage) {
			ChainedStorage tmp = storage;
			swap(tmp);
			return *this;
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
