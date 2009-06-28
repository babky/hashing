#ifndef STORAGE_STATISTICS_H
#define STORAGE_STATISTICS_H

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
		size_t getMinimalChainLength(void) const;

		/**
		 * Length of the longest chain.
		 *
		 * @return Length of the longest chain.
		 */
		size_t getMaximalChainLength(void) const;

		/**
		 * Adds chain to the statistics.
		 *
		 * @param chainLength Added chain - its length.
		 */
		void addChain(size_t chainLength);

		/** 
		 * Clears the statistics.
		 */
		void clear(void);

	private:
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
	};

} }

#endif /* STORAGE_STATISTICS_H */