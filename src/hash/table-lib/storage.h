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
		static const size_t INITIAL_STORAGE_SIZE;

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
	 * Provides the possibility of setting of the values for the storage info.
	 */
	template <class BaseInfo>
	class SettableStorageInfo : public BaseInfo {
			using BaseInfo::elementCount;
			using BaseInfo::tableSize;
	public:
		/**
		 * Underlying storage info.
		 */
		typedef BaseInfo StorageInfo;

		/**
		 * C-tor.
		 */
		explicit SettableStorageInfo(size_t aTableSize):
		  BaseInfo(aTableSize)
		{
		}

		/**
		 * Sets the number of stored elements inside the table.
		 *
		 * @param aElementCount The number of elements.
		 */
		void setElementCount(size_t aElementCount) {
			elementCount = aElementCount;
		}

		/**
		 * Increments the number of stored elements.
		 */
		void incElementCount(void) {
			++elementCount;
		}
		
		/**
		 * Decrements the number of stored elements.
		 */
		void decElementCount(void) {
			--elementCount;
		}

		/**
		 * Sets the table size.
		 *
		 * @param aTableSize Size of the storage.
		 */
		void setTableSize(size_t aTableSize) {
			tableSize = aTableSize;
		}
	};

	/**
	 * Provides the basic information about the hash table.
	 */
	class PlainStorageInfo {
	public:
		/**
		 * C-tor.
		 */
		PlainStorageInfo(size_t aTableSize);

		/**
		 * Number of stored elements retrieval.
		 *
		 * @return Number of elements stored.
		 */
		size_t getElementCount(void) const;

		/**
		 * Length of the table retrieval.
		 *
		 * @return Length of the table retrieval.
		 */
		size_t getTableSize(void) const;

		/**
		 * Load factor retrieval.
		 *
		 * @return Load factor.
		 */
		double getLoadFactor(void) const;

	protected:
		size_t tableSize;
		size_t elementCount;
	};

	/**
	 * Provides the basic information about the hash table with the max chain length.
	 */
	class MaxChainLengthStorageInfo : public PlainStorageInfo {
	public:
		/**
		 * C-tor.
		 */
		explicit MaxChainLengthStorageInfo(size_t aTableSize);

		/**
		 * Load factor retrieval.
		 *
		 * @return Load factor.
		 */
		size_t getMaxChainLength(void) const;

	protected:
		size_t maxChainLength;
	};

	/**
	 * Class allowing changing the PlainStorageInfo.
	 */
	class SettablePlainStorageInfo : public SettableStorageInfo<PlainStorageInfo> {
	public:
		/**
		 * Underlying storage info.
		 */
		typedef PlainStorageInfo StorageInfo;

		/**
		 * C-tor.
		 */
		explicit SettablePlainStorageInfo(size_t aTableSize);
	};

	/**
	 * Class allowing changing the MaxChainLengthStorageInfo.
	 */
	class SettableMaxChainLengthStorageInfo : public SettableStorageInfo<MaxChainLengthStorageInfo> {
	public:
		/**
		 * Underlying storage info.
		 */
		typedef MaxChainLengthStorageInfo StorageInfo;

		/**
		 * C-tor.
		 */
		explicit SettableMaxChainLengthStorageInfo(size_t aTableSize);

		/**
		 * Updates the maximal chain length by the length of a new chain.
		 *
		 * @param refinement The length of the emerging chain.
		 */
		void refineMaxChainLength(size_t refinement);
	};

	/**
	 * Interface for the storage.
	 */
	template <typename T, typename Comparer, typename Hash, class StorageInfo>
	class Storage {
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
		 * Number of stored elements retrieval.
		 *
		 * @return Number of the stored elements.
		 */
		virtual size_t size(void) const = 0;

		/**
		 * Comparer retrieval.
		 *
		 * @return Comparer used.
		 */
		virtual Comparer getComparer(void) const = 0;

		/**
		 * Minimality check, checks if the table has the minimal possible size. It is the size of the table just after
		 * initialization.
		 *
		 * @return Minimality status of the table.
		 */
		virtual bool isMinimal(void) const = 0;

		/**
		 * Current storage info.
		 *
		 * @return Storage information.
		 */
		virtual const StorageInfo & getStorageInfo(void) const = 0;

		/**
		 * Computes the statistics about the storage info. May take O(n) time.
		 *
		 * @param stats Place where the statistics are computed. The stats is cleared prior computation.
		 */
		virtual void computeStatistics(Utils::StorageStatistics & stats) const = 0;
	};

}

#endif /* STORAGE_H */
