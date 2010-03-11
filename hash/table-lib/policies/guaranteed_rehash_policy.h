#ifndef GUARANTEED_REHASH_POLICY_H
#define GUARANTEED_REHASH_POLICY_H

#include "../storage.h"
#include "rehash_policy.h"

namespace Hash { namespace Policies { namespace Rehash {

	class GuaranteedRehashPolicy : public RehashPolicy {
	public:
		GuaranteedRehashPolicy(bool allowDelete = true);

		double getMinLoadFactor(void) const;
		double getMaxLoadFactor(void) const;
		size_t getMaxChainLength(size_t n) const;
		
		bool needsRehashingAfterDelete(const Hash::StorageInfo & storageInfo);
		bool needsRehashingAfterInsert(const Hash::StorageInfo & storageInfo);

	private:
		bool allowDelete;
		double maxLoadFactor;
		double minLoadFactor;
		double multiplicativeConstant;

		static const double MIN_LOAD_FACTOR_NO_DELETE;
		static const double MAX_LOAD_FACTOR_NO_DELETE;
		static const double MULTIPLICATIVE_CONSTANT_NO_DELETE;

		static const double MIN_LOAD_FACTOR_ALLOW_DELETE;
		static const double MAX_LOAD_FACTOR_ALLOW_DELETE;
		static const double MULTIPLICATIVE_CONSTANT_ALLOW_DELETE;

		static const size_t MINIMAL_ALLOWED_BOUND;
	};

} } }

#endif /* GUARANTEED_REHASH_POLICY_H */