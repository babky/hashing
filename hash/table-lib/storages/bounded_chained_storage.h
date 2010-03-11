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
	public:
		typedef Comparer EqualityComparer;
		
		typedef T HashType;
		
		void init_stats(void) {
			this->currentStats.setTableLength(this->getTableSize());
		}

		/**
		 * Chained storage c-tor.
		 */
		BoundedChainedStorage(void):
		  ChainedStorage<T, Comparer, Hash>() {
			  this->init_stats();
		}

		/**
		 * Chained storage c-tor.
		 *
		 * @param comparer Used comparer.
		 * @param tableLength Starting length of the table.
		 */
		explicit BoundedChainedStorage(const EqualityComparer & comparer, 
				size_t tableLength = StorageParams::STARTING_STORAGE_SIZE):
		  ChainedStorage<T, Comparer, Hash>(comparer, tableLength) {
			  this->init_stats();
		}

		/**
		 * Chained storage copy c-tor.
		 *
		 * @param storage Copied storage.
		 */
		BoundedChainedStorage(const BoundedChainedStorage<T, Comparer, Hash> & storage):
		  ChainedStorage<T, Comparer, Hash>(storage){
			this->currentStats = storage.currentStats;
		}

		/**
		 * Assignment operator.
		 */
		BoundedChainedStorage & operator = (const BoundedChainedStorage<T, Comparer, Hash> & storage) {
			this->ChainedStorage<T, Comparer, Hash>::operator =(storage);
			this->currentStats = storage.currentStats;
			return *this;
		}

		void insert(const T & item, HashType hash) {
			this->ChainedStorage<T, Comparer, Hash>::insert(item, hash);
			this->currentStats.refineChain(this->getChainLength(hash));
		}

		bool remove(const T & item, HashType hash) {
			bool retVal = this->ChainedStorage<T, Comparer, Hash>::remove(item, hash);

			if (retVal) {
				this->currentStats.removeElement();
			}

			return retVal;
		}

		/**
		 * Maximal chain length retrieval.
		 *
		 * @return Maximal chain length.
		 */
		size_t getMaxChainLength(void) const {
			return this->currentStats.getMaxChainLength();
		}

		void computeStatistics(Utils::StorageStatistics & stats) const {
			stats = this->currentStats;
		}

		/**
		 * Recomputes the whole statistics if wanted.
		 *
		 * @param stats Place where the computed statistics are stored.
		 * @param recompute Flag indicating if the statistics should be fully recomputed.
		 */
		void computeStatistics(Utils::StorageStatistics & stats, bool recompute) {
			if (recompute) {
				this->ChainedStorage<T, Comparer, Hash>::computeStatistics(stats);
			} else {
				this->computeStatistics(stats);
			}
		}

	private:
		class StorageStatistics : public Utils::StorageStatistics {
		public:

			/**
			 * Refines the maximal chain length.
			 * 
			 * @param chainLength length of the currently prolonged chain.
			 */
			void refineChain(size_t chainLength) {
				++this->elementCount;
				if (this->maxChainLength < chainLength) {
					this->maxChainLength = chainLength;
				}
			}

			/**
			 * Length of the table setter.
			 *
			 * @param tableLength New table's length.
			 */
			void setTableLength(size_t tableLength) {
				this->tableLength = tableLength;
			}

			/**
			 * Element removed.
			 */
			void removeElement(void) {
				--this->elementCount;
			}

		};

		/**
		 * Current storage statistics computed for the hash table.
		 */
		StorageStatistics currentStats;

	};

} }

#endif /* BOUNDED_CHAINED_STORAGE_H */
