#ifndef STORAGE_H
#define STORAGE_H

#include <algorithm>
#include "exceptions/item_stored_exception.h"
#include "utils/storage_statistics.h"

namespace Hash {

	/**
	 * Default settings for the storage.
	 */
	class StorageParams {
	public:

		/**
		 * Default size for the storage table.
		 */
		static const size_t STARTING_STORAGE_SIZE;

	private:
		/**
		 * This class can not be instantiated.
		 */
		StorageParams(void);

		/**
		 * This class can not be instantiated.
		 */
		StorageParams(const StorageParams &);

		/**
		 * This class can not be instantiated.
		 */
		StorageParams & operator =(const StorageParams &);

	};

	/**
	 * Storage information retrieval. One part of the storage.
	 */
	class StorageInfo {
	public:
		/**
		 * Number of stored elements retrieval.
		 *
		 * @return Number of elements stored.
		 */
		virtual size_t getSize(void) const = 0;

		/**
		 * Length of the table retrieval.
		 *
		 * @return Length of the table retrieval.
		 */
		virtual size_t getTableSize(void) const = 0;

		/**
		 * Load factor retrieval.
		 *
		 * @return Load factor.
		 */
		virtual double getLoadFactor(void) const = 0;

		/**
		 * Computes the statistics for this storage.
		 *
		 * @param stats Storage for statistics.
		 */ 
		virtual void computeStatistics(Utils::StorageStatistics & stats) const = 0;
	};

	/**
	 * Interface for the storage.
	 */
	template <typename T, typename Comparer, typename Hash>
	class Storage : public StorageInfo {
	public:
		/**
		 * Inserts the {@code element} into the table.
		 *
		 * @param element Added element.
		 * @param hash Hash of the element.
		 */
		virtual void insert(const T & item, Hash hash) = 0;

		/**
		 * Removes the {@code element} from the table.
		 *
		 * @param element Removed element.
		 * @param hash Hash of the element.
		 */
		virtual bool remove(const T & item, Hash hash) = 0;

		/**
		 * Containment check.
		 *
		 * @param element Element to be checked for.
		 * @param hash Hash of the element.
		 *
		 * @return Containment check.
		 */
		virtual bool contains(const T & item, Hash hash) const = 0;

		/**
		 * Clears the storage.
		 */
		virtual void clear(void) = 0;

		/**
		 * Comparer retrieval.
		 *
		 * @return Comparer used.
		 */
		virtual Comparer getComparer(void) const = 0;

		/**
		 * Minimality check.
		 *
		 * @return Minimality status of the table.
		 */
		virtual bool isMinimal(void) const = 0;
	};

	/**
	 * Typical storage for a hashing table - linear chaining.
	 *
	 * @typeparam T Type of the stored items.
	 * @typeparam Comparer Comparer used.
	 * @typeparam Hash Type of the hash.
	 */
	template <typename T, typename Comparer, typename Hash>
	class ChainedStorage : public Storage<T, Comparer, Hash> {
		friend void swap(ChainedStorage & a, ChainedStorage & b);

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
			swap(*this, tmp);
			return *this;
		}
		
		void insert(const T & item, HashType hash) {
			if (!this->storage[hash].insert(item, this->comparer)) {
				throw ItemStoredException<T>(item);
			}

			++this->elementCount;
		}

		bool remove(const T & item, HashType hash) {
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
			// TODO: assert storageLength != 0
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
			explicit ChainedNode(const T & item, ChainedNode * next = 0) {
				this->item = item;
				this->next = next;
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
			ChainedList(void) {
				this->first = 0;
				this->elementCount = 0;
			}

			/**
			 * List with the first element.
			 *
			 * @param element First item of the list.
			 */
			explicit ChainedList(const T & element) {
				this->elementCount = 1;
				this->first = new ChainedNode(element);
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
					// TODO: assert(hash of item == hash of n->item);
					if (comparer((*n)->item, item)) {
						return false;
					}
				}

				// TODO: assert(n != 0);
				*n = new ChainedNode(item);
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
					// TODO: assert(hash of item == hash of n->item);
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

				return (*this);
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

		/**
		 * Swapping of the two storages.
		 *
		 * @param a Storage to be swapped.
		 * @param b Storage to be swapped.
		 */
		friend void swap(ChainedStorage<T, EqualityComparer, HashType> & a, ChainedStorage<T, EqualityComparer, HashType> & b) {
			std::swap(a.storage, b.storage);
			std::swap(a.elementCount, b.elementCount);
			std::swap(a.storageLength, b.storageLength);
		}
		
	};

}

#endif /* STORAGE_H */