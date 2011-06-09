#ifndef REHASH_POLICY_H
#define REHASH_POLICY_H

#include "../storage.h"

namespace Hash { namespace Policies { namespace Rehash {

	/**
	 * Standard interface for the rehashing policy.
	 */
	class RehashPolicy {
	public:
		/**
		 * Check for the rehashing need after deletition.
		 *
		 * @return Rehashing need.
		 */
		virtual bool needsRehashingAfterDelete(const Hash::StorageInfo & storageInfo) = 0;

		/**
		 * Check for the rehashing need after insertion.
		 *
		 * @return Rehashing need.
		 */
		virtual bool needsRehashingAfterInsert(const Hash::StorageInfo & storageInfo) = 0;
	};

	/**
	 * Standard rehashing policy which keeps the table load factor inside given bounds.
	 */
	class LoadFactorBoundsRehashPolicy : public RehashPolicy {
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
		explicit LoadFactorBoundsRehashPolicy(double minFactor = DEFAULT_MIN_LOAD_FACTOR, double maxFactor = DEFAULT_MAX_LOAD_FACTOR);

		/**
		 * Minimal load factor retrieval.
		 *
		 * @return Minimal load factor.
		 */
		double getMinLoadFactor(void) const;
		
		/**
		 * Maximal load factor retrieval.
		 *
		 * @return Maximal load factor.
		 */
		double getMaxLoadFactor(void) const;

		bool needsRehashingAfterInsert(const Hash::StorageInfo & storageInfo);
		bool needsRehashingAfterDelete(const Hash::StorageInfo & storageInfo);

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
