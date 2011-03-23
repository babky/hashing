#ifndef CHAINED_STORAGE_H
#define CHAINED_STORAGE_H

#include <algorithm>
#include "utils/chain_length_aware_storage_info.h"
#include "storage.h"

namespace Hash { namespace Storages {

	/**
	 * Typical storage for a hashing table - separate chaining.
	 *
	 * @typeparam T Type of the stored items.
	 * @typeparam Comparer Comparer used.
	 * @typeparam Hash Type of the hash.
	 */
	template <typename T, typename Comparer, typename Hash>
	class ChainedStorage : public Storage<T, Comparer, Hash, Utils::ChainLengthAwareStorageInfo> {
	public:
		/**
		 * Storage type used.
		 */
		typedef ChainedStorage<T, Comparer, Hash> Storage;

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

		/**
		 * Chained storage c-tor.
		 */
		ChainedStorage(void):
		  elementCount(0),
		  storageLength(StorageParams::STARTING_STORAGE_SIZE),
		  storage(new StorageItem[StorageParams::STARTING_STORAGE_SIZE]),
		  comparer(EqualityComparer()) {
		}

		/**
		 * Chained storage c-tor.
		 *
		 * @param comparer Used comparer.
		 * @param tableLength Starting length of the table.
		 */
		explicit ChainedStorage(const EqualityComparer & comparer, size_t tableLength = StorageParams::STARTING_STORAGE_SIZE):
		  elementCount(0),
		  storageLength(tableLength),
		  storage(new StorageItem[tableLength]),
		  comparer(comparer) {
		}

		/**
		 * Chained storage d-tor.
		 */
		~ChainedStorage(void) {
			delete [] this->storage;
			this->storage = 0;
			this->storageLength = 0;
		}

		/**
		 * Chained storage copy c-tor.
		 *
		 * @param storage Copied storage.
		 */
		ChainedStorage(const ChainedStorage & storage) {
			this->storage = new StorageItem[storage.storageLength];
			this->storageLength = storage.storageLength;
			this->elementCount = storage.elementCount;
			this->comparer = storage.comparer;
		}

		/**
		 * Chained storage assignement operator.
		 *
		 * @param storage Copied storage.
		 */
		ChainedStorage & operator =(const ChainedStorage & storage) {
			ChainedStorage tmp = storage;
			swap(tmp);
			return *this;
		}

		void insert(const T & item, HashType hash) {
			simple_assert(hash < storageLength, "Hash must be inside the storage!");

			if (!this->storage[hash].insert(item, this->comparer)) {
				throw ItemStoredException<T>(item);
			}

			++this->elementCount;
		}

		bool remove(const T & item, HashType hash) {
			simple_assert(hash < storageLength, "Hash must be inside the storage!");

			if (this->storage[hash].remove(item, this->comparer)) {
				--this->elementCount;
				return true;
			} else {
				return false;
			}
		}

		bool contains(const T & item, HashType hash) const {
			return this->storage[hash].contains(item, this->comparer);
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
			for (size_t i = 0; i < this->storageLength; ++i) {
				stats.addChain(this->storage[i].getSize());
			}
		}

		bool isMinimal(void) const {
			return this->getTableSize() <= StorageParams::STARTING_STORAGE_SIZE;
		}

		Comparer getComparer(void) const {
			return this->comparer;
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
				delete this->next;
				this->next = 0;
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

		// TODO: doc
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
				std::swap(a.first, b.first);
				std::swap(a.elementCount, b.elementCount);
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
			ChainedList(const ChainedList & list) {
				this->elementCount = list->elementCount;
				for (ChainedNode * n = list->first; n->next; n = n->next) {
					this->first = new ChainedNode(n->item, this->first);
				}
			}

			/**
			 * Assignment operator.
			 */
			ChainedList & operator =(const ChainedList & list) {
				ChainedList tmp = list;
				swap(*this, tmp);
				return this;
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
			ChainedStorageIterator(const Storage * storage, bool beginning):
			  storage(storage),
			  chainIndex(beginning ? 0 : storage->getTableSize()),
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
				size_t tableSize = this->storage->getTableSize();
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
			const Storage * storage;
			size_t chainIndex;
			ChainIterator chainIterator;
		};

	private:
		/**
		 * Table of chains.
		 */
		typedef ChainedList StorageItem;

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
