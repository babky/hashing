#ifndef BOUNDED_CHAINED_STORAGE_H
#define BOUNDED_CHAINED_STORAGE_H

#include "storages/chained_storage.h"

namespace Hash { namespace Storages {

	/**
	 * Typical storage for a hashing table - seperate chaining. Tracks the information about the lengths of the chain
	 * lenghts.
	 *
	 * @typeparam T Type of the stored items.
	 * @typeparam Comparer Comparer used.
	 * @typeparam Hash Type of the hash.
	 */
	template <typename T, typename Comparer, typename Hash>
	class BoundedChainedStorage : public ChainedStorageBase<T, Comparer, Hash, SettableMaxChainLengthStorageInfo> {
	public:
		typedef Comparer EqualityComparer;
		
		typedef T HashType;

	private:
		typedef ChainedStorageBase<T, Comparer, Hash, SettableMaxChainLengthStorageInfo> BaseStorage;

	public:
		/**
		 * Chained storage c-tor.
		 *
		 * @param tableSize Initial table size.
		 */
		explicit BoundedChainedStorage(size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE):
		  BaseStorage(tableSize) {
		}

		/**
		 * Chained storage c-tor.
		 *
		 * @param comparer Used comparer.
		 * @param tableLength Starting length of the table.
		 */
		explicit BoundedChainedStorage(const EqualityComparer & comparer, 
			size_t tableLength = StorageParams::INITIAL_STORAGE_SIZE):
		  BaseStorage(comparer, tableLength) {
		}

		/**
		 * Chained storage copy c-tor.
		 *
		 * @param storage Copied storage.
		 */
		BoundedChainedStorage(const BoundedChainedStorage<T, Comparer, Hash> & storage):
		  BaseStorage(storage){
		}

		/**
		 * Assignment operator.
		 */
		BoundedChainedStorage & operator = (const BoundedChainedStorage<T, Comparer, Hash> & storage) {
			this->BaseStorage::operator =(storage);
			return *this;
		}

		void insert(const T & item, HashType hash) {
			this->BaseStorage::insert(item, hash);
			this->storageInfo.refineMaxChainLength(this->getChainLength(hash));
		}

		void swap(BoundedChainedStorage & b) {
			BaseStorage::swap(b);
		}
		
		friend void swap(BoundedChainedStorage & a, BoundedChainedStorage & b) {
			a.swap(b);
		}
	};

} }

namespace std {

	template <typename T, typename Comparer, typename HashType>
	void swap(Hash::Storages::BoundedChainedStorage<T, Comparer, HashType> & a, Hash::Storages::BoundedChainedStorage<T, Comparer, HashType> & b) {
		a.swap(b);
	}

}

#endif /* BOUNDED_CHAINED_STORAGE_H */
