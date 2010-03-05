#ifndef BOUNDED_CHAINED_STORAGE_H
#define BOUNDED_CHAINED_STORAGE_H

#include "storages/chained_storage.h"

namespace Hash { namespace Storages {

	/**
	 * Typical storage for a hashing table - linear chaining.
	 *
	 * @typeparam T Type of the stored items.
	 * @typeparam Comparer Comparer used.
	 * @typeparam Hash Type of the hash.
	 */
	template <typename T, typename Comparer, typename Hash>
	class BoundedChainedStorage : public ChainedStorage<T, Comparer, Hash> {

		/**
		 * Current storage statistics computed for the hash table.
		 */
		size_t maxChainLength;

		void insert(const T & item, HashType hash) {
			this->ChainedStorage::insert(item, hash);

			size_t chainLength = this->storage[hash].getSize();
			if (chainLength > this->maxChainLength) {
				this->maxChainLength = chainLength;
			}
		}

	}

} }

#endif /* BOUNDED_CHAINED_STORAGE_H */
