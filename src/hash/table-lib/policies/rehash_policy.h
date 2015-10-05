#ifndef REHASH_POLICY_H
#define REHASH_POLICY_H

#include "../storage.h"

namespace Hash { namespace Policies { namespace Rehash {

	/**
	 * Standard interface for the rehashing policy.
	 */
	template<typename StorageInfoType>
	class RehashPolicy {
	public:
#ifdef HASH_DEBUG
		/**
		 * Check for the rehashing need after deletition.
		 *
		 * @return Rehashing need.
		 */
		virtual bool needsRehashingAfterDelete(const StorageInfoType & storageInfo) = 0;

		/**
		 * Check for the rehashing need after insertion.
		 *
		 * @return Rehashing need.
		 */
		virtual bool needsRehashingAfterInsert(const StorageInfoType & storageInfo) = 0;
#endif
	};

	/**
	 * Standard rehashing policy which keeps the table load factor inside given bounds.
	 */
	class LoadFactorBoundsRehashPolicy : public RehashPolicy<PlainStorageInfo> {
	public:
		/**
		 * Default minimal load factor.
		 */
		const static double DEFAULT_MIN_LOAD_FACTOR;

		/**
		 * Default maximal load factor.
		 */
		const static double DEFAULT_MAX_LOAD_FACTOR;

		/**
		 * Creates the policy with given bounds.
		 *
		 * @param minFactor Minimal load factor to be kept.
		 * @param maxFactor Maximal load factor to be kept.
		 */
		explicit LoadFactorBoundsRehashPolicy(double aMinFactor = DEFAULT_MIN_LOAD_FACTOR, double aMaxFactor = DEFAULT_MAX_LOAD_FACTOR):
		  minFactor(aMinFactor),
		  maxFactor(aMaxFactor) {
		}

		/**
		 * Minimal load factor retrieval.
		 *
		 * @return Minimal load factor.
		 */
		inline double getMinLoadFactor(void) const {
			return this->minFactor;
		}
		
		/**
		 * Maximal load factor retrieval.
		 *
		 * @return Maximal load factor.
		 */
		inline double getMaxLoadFactor(void) const {
			return this->maxFactor;
		}

		inline bool needsRehashingAfterInsert(const PlainStorageInfo & storageInfo) {
			return storageInfo.getElementCount() > this->getMaxLoadFactor() * storageInfo.getTableSize();
		}

		inline bool needsRehashingAfterDelete(const PlainStorageInfo & storageInfo) {
			return storageInfo.getElementCount() < this->getMinLoadFactor() * storageInfo.getTableSize();
		}

	private:
		/**
		 * Minimal load factor.
		 */
		double minFactor;

		/**
		 * Maximal load factor.
		 */
		double maxFactor;
	};

} } }

#endif /* REHASH_POLICY_H */
