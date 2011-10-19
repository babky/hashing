#ifndef STORAGE_STATISTICS_H
#define STORAGE_STATISTICS_H

#include <ostream>

namespace Hash { namespace Utils {

	/**
	 * Storage statistics for the hash table storage.
	 */
	class StorageStatistics {
	public:
		/**
		 * Creates the empty statistics.
		 */ 
		StorageStatistics(void);

		/**
		 * Number of stored items.
		 *
		 * @return Number of stored items inside the table.
		 */
		size_t getElementCount(void) const;

		/**
		 * Length of the array/dictionary/storage.
		 *
		 * @return Length of the array/dictionary/storage.
		 */
		size_t getTableLength(void) const;

		/**
		 * Explicitly sets the length of the hash table.
		 */
		void setTableLength(size_t length);

		/**
		 * Load factor of the table retrieval.
		 *
		 * @return Table's load factor.
		 */
		double getLoadFactor(void) const;

		/**
		 * Length of the shortest chain.
		 *
		 * @return Length of the shortest chain.
		 */
		size_t getMinChainLength(void) const;

		/**
		 * Length of the longest chain.
		 *
		 * @return Length of the longest chain.
		 */
		size_t getMaxChainLength(void) const;

		/**
		 * Adds chain to the statistics.
		 *
		 * @param chainLength Added chain - its length.
		 */
		void addChain(size_t chainLength);

		/**
		 * Rehashing done.
		 */
		void rehash(void);

		/**
		 * Rehashing count retrieval.
		 * 
		 * @return Rehashing count.
		 */
		size_t getRehashCount(void) const;

		/** 
		 * Clears the statistics.
		 */
		void clear(void);

	protected:
		/**
		 * Number of elements stored inside the table.
		 */
		size_t elementCount;

		/**
		 * Length of the array/dictionary.
		 */
		size_t tableLength;

		/**
		 * Length of the shortest chain.
		 */
		size_t minChainLength;

		/**
		 * Length of the longest chain.
		 */
		size_t maxChainLength;

		/**
		 * Rehash count.
		 */
		size_t rehashCount;
	};

	/**
	 * Prints the storage stats.
	 *
	 * @param stats Statistics to be outputted.
	 * @param out	Output stream.
	 * @return		Output stream.
	 */
	std::ostream & operator <<(std::ostream & out, const StorageStatistics & stats);

} }

#endif /* STORAGE_STATISTICS_H */