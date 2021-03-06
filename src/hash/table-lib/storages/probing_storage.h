#ifndef PROBING_STORAGE_H
#define PROBING_STORAGE_H

#include <algorithm>
#include "exceptions/item_stored_exception.h"
#include "storage.h"
#include "utils/hash_assert.h"
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
	class ProbingStorage : public Storage<T, Comparer, Hash, PlainStorageInfo> {
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
		 * Probing storage c-tor.
		 *
		 * @param comparer Used comparer.
		 * @param tableSize Starting length of the table.
		 */
		explicit ProbingStorage(size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE):
		  storageInfo(tableSize),
		  storage(new StorageItem[tableSize]),
		  comparer(EqualityComparer())
		{
		}

		/**
		 * Probing storage c-tor.
		 *
		 * @param comparer Used comparer.
		 * @param tableSize Starting length of the table.
		 */
		explicit ProbingStorage(const EqualityComparer & comparer, 
			size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE):
		  storageInfo(tableSize),
		  storage(new StorageItem[tableSize]),
		  comparer(comparer)
		{
		}

		/**
		 * Probing storage d-tor.
		 */
		~ProbingStorage(void) {
			delete [] this->storage;
			this->storage = 0;
		}

		/**
		 * Chained storage copy c-tor.
		 *
		 * @param storage Copied storage.
		 */
		ProbingStorage(const ProbingStorage & aStorage):
		  storageInfo(aStorage.storageInfo),
		  comparer(aStorage.comparer),
		  probingPolicy(aStorage.probingPolicy) {
			size_t tableSize = storageInfo.getTableSize();
			this->storage = new StorageItem[tableSize];
			for (size_t i = 0; i < tableSize; ++i) {
				this->storage[i] = aStorage.storage[i];
			}
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

		inline void insert(const T & item, HashType hash) {
			size_t tableSize = storageInfo.getTableSize();
			simple_assert(hash < tableSize, "Hash must be inside the storage!");
			
			HashType pos = hash;
			for (size_t i = 0; ; ++i) {
				pos = probingPolicy.probe(hash, i) % tableSize;

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
			this->storageInfo.incElementCount();
		}

		static const bool HAS_REHASH = true;

		template<class Function>
		void rehash(ProbingStorage & n, Function & f) {
			for (size_t i = 0, e = getTableSize(); i < e; ++i) {
				if (storage[i].empty) {
					continue;
				}

				n.insert(storage[i].item, f.Function::hash(storage[i].item));
			}
		}

		inline bool remove(const T & item, HashType hash) {
			size_t tableSize = storageInfo.getTableSize();
			simple_assert(hash < tableSize, "Hash must be inside the storage!");

			HashType pos = hash;
			for (size_t i = 0; ; ++i) {
				pos = probingPolicy.probe(hash, i) % tableSize;

				if (this->comparer(item, this->storage[pos].item)) {
					this->storage[pos].empty = true;
					this->storageInfo.decElementCount();
					return true;
				}

				if (!this->storage[pos].bridge) {
					return false;
				}
			};

			return false;
		}

		inline bool contains(const T & item, HashType hash) const {
			size_t tableSize = storageInfo.getTableSize();
			simple_assert(hash < tableSize, "Hash must be inside the storage!");

			HashType pos = hash;
			for (size_t i = 0; ; ++i) {
				pos = probingPolicy.probe(hash, i) % tableSize;

				if (!this->storage[pos].empty && this->comparer(item, this->storage[pos].item)) {
					return true;
				}

				if (!this->storage[pos].bridge) {
					return false;
				}
			};

			return false;
		}

		size_t size(void) const {
			return this->storageInfo.getElementCount();
		}

		inline size_t getTableSize(void) const {
			return this->storageInfo.getTableSize();
		}

		void clear(void) {
			delete [] this->storage;
			this->storage = new StorageItem[StorageParams::INITIAL_STORAGE_SIZE];
			this->storageInfo.setTableSize(StorageParams::INITIAL_STORAGE_SIZE);
		}

		inline double getLoadFactor(void) const {
			simple_assert(this->storageInfo.getTableSize() != 0, "Storage must contain at least one slot.");
			return static_cast<double>(this->elementCount) / this->storageInfo.getTableSize();
		}

		void computeStatistics(Utils::StorageStatistics & stats) const {
			stats.clear();
			
			size_t tableSize = storageInfo.getTableSize();
			size_t psl = 0;
			for (size_t i = 0; i < tableSize; ++i) {
				if (storage[i].empty) {
					stats.addChain(psl);
					psl = 0;
				} else {
					++psl;
				}
			}
			stats.addChain(psl);

			stats.setTableLength(tableSize);
		}

		bool isMinimal(void) const {
			return this->getTableSize() <= StorageParams::INITIAL_STORAGE_SIZE;
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

		const PlainStorageInfo & getStorageInfo(void) const {
			return storageInfo;
		}

	private:
		/**
		 * Single item for representing a stored element.
		 */
		struct StorageItem {
			StorageItem(void):
			  empty(true),
			  bridge(false) {
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
		 * Current info.
		 */
		SettablePlainStorageInfo storageInfo;

		/**
		 * Table of chains.
		 */
		StorageItem * storage;

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

			inline friend bool operator ==(const ProbingStorageIterator & a, const ProbingStorageIterator & b) {
				return &(*a) == &(*b); 
			}

			inline friend bool operator !=(const ProbingStorageIterator & a, const ProbingStorageIterator & b) {
				return !(a == b);
			}

			inline T & operator *(void) {
				return storage->storage[position].item;
			}

			inline const T & operator *(void) const {
				return const_cast<ProbingStorageIterator *> (this)->storage->storage[position].item;
			}

			inline ProbingStorageIterator operator ++(void) {
				size_t tableSize = storage->storageInfo.getTableSize();
				for (; ;) {
					// Out of bounds?
					if (position >= tableSize) {
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
			swap(storageInfo, b.storageInfo);
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
		typedef typename ProbingStorage<T, Comparer, Hash, LinearProbingPolicy>::EqualityComparer EqualityComparer;

		explicit LinearProbingStorage(size_t tableLength = StorageParams::INITIAL_STORAGE_SIZE):
		  ProbingStorage<T, Comparer, Hash, LinearProbingPolicy>(tableLength)
		{
		}

		explicit LinearProbingStorage(const EqualityComparer & comparer, 
			size_t tableLength = StorageParams::INITIAL_STORAGE_SIZE):
		  ProbingStorage<T, Comparer, Hash, LinearProbingPolicy>(comparer, tableLength)
		{
		}
	};

	template<typename T, class Comparer, typename Hash>
	class QuadraticProbingStorage : public ProbingStorage<T, Comparer, Hash, QuadraticProbingPolicy> {
	public:
		typedef QuadraticProbingStorage<T, Comparer, Hash> Storage;
		typedef typename ProbingStorage<T, Comparer, Hash, QuadraticProbingPolicy>::EqualityComparer EqualityComparer;

		explicit QuadraticProbingStorage(size_t tableLength = StorageParams::INITIAL_STORAGE_SIZE):
		  ProbingStorage<T, Comparer, Hash, QuadraticProbingPolicy>(tableLength)
		{
		}

		explicit QuadraticProbingStorage(const EqualityComparer & comparer, 
			size_t tableLength = StorageParams::INITIAL_STORAGE_SIZE):
		  ProbingStorage<T, Comparer, Hash, QuadraticProbingPolicy>(comparer, tableLength)
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
