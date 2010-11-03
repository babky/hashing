#ifndef WORST_CASE_REHASH_POLICY_H
#define WORST_CASE_REHASH_POLICY_H

#include "../storage.h"
#include "rehash_policy.h"

namespace Hash { namespace Policies { namespace Rehash {

	/**
	 * Standard rehashing policy which keeps the table load factor inside given bounds.
	 */
	class WorstCaseRehashPolicy : public RehashPolicy {
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
		 * Factor for the allowed ELPSL.
		 */
		const static size_t ELPSL_FACTOR;

		/**
		 * Constant for the ELPSL.
		 */
		const static size_t ELPSL_CONSTANT;

		/**
		 * Creates the policy with given bounds.
		 *
		 * @param minFactor Minimal load factor to be kept.
		 * @param maxFactor Maximal load factor to be kept.
		 */
		explicit WorstCaseRehashPolicy(double minFactor = DEFAULT_MIN_LOAD_FACTOR, double maxFactor = DEFAULT_MAX_LOAD_FACTOR);

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

#endif /* WORST_CASE_REHASH_POLICY_H */
