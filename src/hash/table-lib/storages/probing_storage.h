#ifndef PROBING_STORAGE_H
#define PROBING_STORAGE_H

#include <algorithm>
#include "utils/chain_length_aware_storage_info.h"
#include "exceptions/item_stored_exception.h"
#include "storage.h"
#include "probing/policies.h"

namespace Hash { namespace Storages {

	/**
	 * Probing storage for hash table.
	 *
	 * @typeparam T Type of the stored items.
	 * @typeparam Comparer Comparer used.
	 * @typeparam Hash Type of the hash.
	 * @typeparam ProbingPolicy Policy of probing (linear, quadratic, etc.)
	 */
	template<
		typename T, 
		typename Comparer, 
		typename Hash, 
		template <class> class ProbingPolicy
	>
	class ProbingStorage : public Storage<T, Comparer, Hash> {
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
		  comparer(EqualityComparer())
		{
		}

		/**
		 * Probing storage c-tor.
		 *
		 * @param comparer Used comparer.
		 * @param tableLength Starting length of the table.
		 */
		explicit ProbingStorage(const EqualityComparer & comparer, 
			size_t tableLength = StorageParams::STARTING_STORAGE_SIZE):
		  elementCount(0),
		  storageLength(tableLength),
		  storage(new StorageItem[tableLength]),
		  comparer(comparer)
		{
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
			
			HashType pos;
			for (size_t i = 0; ; ++i) {
				pos = probingPolicy.probe(hash, i) % storageLength;

				if (this->storage[pos].empty) {
					break;
				}

				if (this->comparer(item, this->storage[pos].item)) {
					throw ItemStoredException<T>(item);
				}
			}

			this->storage[pos].empty = false;
			this->storage[pos].bridge = true;
			this->storage[pos].item = item;
			++this->elementCount;
		}

		bool remove(const T & item, HashType hash) {
			simple_assert(hash < storageLength, "Hash must be inside the storage!");

			HashType pos;
			for (size_t i = 0; ; ++i) {
				pos = probingPolicy.probe(hash, i) % storageLength;

				if (this->comparer(item, this->storage[pos].item)) {
					this->storage[pos].empty = true;
					--this->elementCount;
					return true;
				}

				if (!this->storage[pos].bridge) {
					return false;
				}
			};
		}

		bool contains(const T & item, HashType hash) const {
			simple_assert(hash < storageLength, "Hash must be inside the storage!");

			HashType pos;
			for (size_t i = 0; ; ++i) {
				pos = probingPolicy.probe(hash, i) % storageLength;

				if (!this->storage[pos].empty && this->comparer(item, this->storage[pos].item)) {
					return true;
				}

				if (!this->storage[pos].bridge) {
					return false;
				}
			};
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
			stats.addChain(this->getSize());
			stats.setTableLength(this->getTableSize());
		}

		bool isMinimal(void) const {
			return this->getTableSize() <= StorageParams::STARTING_STORAGE_SIZE;
		}

		Comparer getComparer(void) const {
			return this->comparer;
		}

		Iterator getBeginning(void) {
			return ProbingStorageIterator(this, 0);
		}

		Iterator getEnd(void) {
			return ProbingStorageIterator(this, getTableSize());
		}

	private:
		/**
		 * Single item for representing a stored element.
		 */
		struct StorageItem {
			StorageItem(void):
			  empty(true),
			  bridge(false)
			{
			}

			/**
			 * Flag indicating if the item is empty.
			 */
			bool empty;

			/**
			 * Flag indicating if the item is empty but the chain contains something behind.
			 */
			bool bridge;

			/**
			 * Represented element.
			 */
			T item;
		};

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
		 * Probing policy.
		 */
		ProbingPolicy<Hash> probingPolicy;

	public:
		class ProbingStorageIterator {
		public:
			explicit ProbingStorageIterator(ProbingStorage * aStorage, HashType aPosition = 0):
			  storage(aStorage),
			  position(aPosition)
			{
				if (storage->storage[position].empty) {
					++(*this);
				}
			}

			friend bool operator ==(const ProbingStorageIterator & a, const ProbingStorageIterator & b) {
				return a.position == b.position && a.storage == b.storage;
			}

			friend bool operator !=(const ProbingStorageIterator & a, const ProbingStorageIterator & b) {
				return !(a == b);
			}

			T operator *(void) const {
				return storage->storage[position].item;
			}

			ProbingStorageIterator operator ++(void) {
				for (; ;) {
					// Out of bounds?
					if (position >= storage->storageLength) {
						break;
					}

					// Move.
					++position;

					// Found an element?
					if (!storage->storage[position].empty) {
						break;
					}
				}

				return *this;
			}

			ProbingStorageIterator operator ++(int) {
				ProbingStorageIterator tmp = *this;
				++(*this);
				return tmp;
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
		void swap(ProbingStorage<T, EqualityComparer, HashType, ProbingPolicy> & b) {
			using std::swap;

			swap(storage, b.storage);
			swap(comparer, b.comparer);
			swap(elementCount, b.elementCount);
			swap(storageLength, b.storageLength);
			swap(probingPolicy, b.probingPolicy);
		}

		friend void swap(ProbingStorage & a, ProbingStorage & b) {
			a.swap(b);
		}

	};

	template<typename T, class Comparer, typename Hash>
	class LinearProbingStorage : public ProbingStorage<T, Comparer, Hash, LinearProbingPolicy> {
	public:
		typedef LinearProbingStorage<T, Comparer, Hash> Storage;
		using ProbingStorage<T, Comparer, Hash, LinearProbingPolicy>::EqualityComparer;

		LinearProbingStorage(void) {
		}

		explicit LinearProbingStorage(const EqualityComparer & comparer, 
			size_t tableLength = StorageParams::STARTING_STORAGE_SIZE):
		  ProbingStorage(comparer, tableLength)
		{
		}
	};

	template<typename T, class Comparer, typename Hash>
	class QuadraticProbingStorage : public ProbingStorage<T, Comparer, Hash, QuadraticProbingPolicy> {
	public:
		typedef QuadraticProbingStorage<T, Comparer, Hash> Storage;
		using ProbingStorage<T, Comparer, Hash, QuadraticProbingPolicy>::EqualityComparer;

		QuadraticProbingStorage(void) {
		}

		explicit QuadraticProbingStorage(const EqualityComparer & comparer, 
			size_t tableLength = StorageParams::STARTING_STORAGE_SIZE):
		  ProbingStorage(comparer, tableLength)
		{
		}
	};

} }

namespace std {

	template <typename T, typename Comparer, typename HashType, template <class> class ProbingPolicy>
	void swap(Hash::Storages::ProbingStorage<T, Comparer, HashType, ProbingPolicy> & a, 
			Hash::Storages::ProbingStorage<T, Comparer, HashType, ProbingPolicy> & b) {
		a.swap(b);
	}

}

#endif /* PROBING_STORAGE_H */
