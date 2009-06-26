#ifndef STORAGE_H
#define STORAGE_H

#include <algorithm>
#include "exceptions/item_stored_exception.h"

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
	 * Typical storage for a hashing table - linear chaining.
	 *
	 * @typeparam T Type of the stored items.
	 * @typeparam Comparer Comparer used.
	 * @typeparam Hash Type of the hash.
	 */
	template <typename T, typename Comparer, typename Hash>
	class ChainedStorage {
		friend void swap(ChainedStorage & a, ChainedStorage & b);

	public:
		/**
		 * Comparer used.
		 */
		typedef Comparer EqualityComparer;

		/**
		 * Type of the hash value.
		 */
		typedef Hash HashType;

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
		 */
		ChainedStorage(const EqualityComparer & comparer):
		  elementCount(0),
		  storageLength(StorageParams::STARTING_STORAGE_SIZE),
		  storage(new StorageItem[StorageParams::STARTING_STORAGE_SIZE]),
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
		
		/**
		 * Inserts the {@code element} into the table.
		 *
		 * @param element Added element.
		 * @param hash Hash of the element.
		 */
		void insert(const T & item, HashType hash) {
			if (!this->storage[hash].insert(item, this->comparer)) {
				throw ItemStoredException<T>(item);
			}

			++this->elementCount;
		}

		/**
		 * Removes the {@code element} from the table.
		 *
		 * @param element Removed element.
		 * @param hash Hash of the element.
		 */
		bool remove(const T & item, HashType hash) {
			if (this->storage[hash].remove(item, this->comparer)) {
				--this->elementCount;
				return true;
			} else {
				return false;
			}
		}

		/**
		 * Containment check.
		 *
		 * @param element Element to be checked for.
		 * @param hash Hash of the element.
		 *
		 * @return Containment check.
		 */
		bool contains(const T & item, HashType hash) const {
			return this->storage[hash].contains(item, this->comparer);
		}

		/**
		 * Number of stored elements retrieval.
		 *
		 * @return Number of elements stored.
		 */
		size_t getSize(void) const {
			return this->elementCount;
		}

		/**
		 * Length of the table retrieval.
		 *
		 * @return Length of the table retrieval.
		 */
		size_t getTableSize(void) const {
			return this->storageLength;
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
				for (ChainedNode * n = list->first, n->next; n = n->next) {
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
		
	};

	/**
	 * Swapping of the two storages.
	 *
	 * @param a Storage to be swapped.
	 * @param b Storage to be swapped.
	 */
	template<typename T, typename EqualityComparer, typename HashType>
	void swap(ChainedStorage<T, EqualityComparer, HashType> & a, ChainedStorage<T, EqualityComparer, HashType> & b) {
		std::swap(a.storage, b.storage);
		std::swap(a.elementCount, b.elementCount);
		std::swap(a.storageLength, b.storageLength);
	}

}

#endif /* STORAGE_H */