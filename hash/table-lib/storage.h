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

}

#endif /* STORAGE_H */
