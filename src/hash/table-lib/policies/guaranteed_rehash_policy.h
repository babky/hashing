#ifndef GUARANTEED_REHASH_POLICY_H
#define GUARANTEED_REHASH_POLICY_H

#include "../storage.h"
#include "rehash_policy.h"

namespace Hash { namespace Policies { namespace Rehash {

	class GuaranteedRehashPolicy : public RehashPolicy<MaxChainLengthStorageInfo> {
	public:
		GuaranteedRehashPolicy(bool allowDelete = true):
		  allowDelete(allowDelete),
		  maxLoadFactor(allowDelete ? MAX_LOAD_FACTOR_ALLOW_DELETE : MAX_LOAD_FACTOR_NO_DELETE), 
		  minLoadFactor(allowDelete ? MIN_LOAD_FACTOR_ALLOW_DELETE : MIN_LOAD_FACTOR_NO_DELETE),
		  multiplicativeConstant(allowDelete ? MULTIPLICATIVE_CONSTANT_ALLOW_DELETE : MULTIPLICATIVE_CONSTANT_NO_DELETE) {
		}

		virtual ~GuaranteedRehashPolicy(void) {
		}

		inline double getMinLoadFactor(void) const{
			return this->minLoadFactor;
		}

		inline double getMaxLoadFactor(void) const {
			return this->maxLoadFactor;
		}

		inline size_t getMaxChainLength(size_t n) const {
			double t = log2(n);
		    size_t maxLength = static_cast<size_t> (ceil(this->multiplicativeConstant * t * log2(t)));
			if (maxLength < MINIMAL_ALLOWED_BOUND) {
				return MINIMAL_ALLOWED_BOUND;
			} else {
				return maxLength;
			}
		}
		
		inline bool needsRehashingAfterDelete(const MaxChainLengthStorageInfo & storageInfo) {
			if (storageInfo.getLoadFactor() < this->getMinLoadFactor()) {
				return true;
			}

			return false;
		}

		inline bool needsRehashingAfterInsert(const MaxChainLengthStorageInfo & storageInfo) {
			if (storageInfo.getLoadFactor() > this->getMaxLoadFactor()) {
				return true;
			}

			if (storageInfo.getMaxChainLength() > this->getMaxChainLength(storageInfo.getElementCount())) {
				return true;
			}

			return false;
		}

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
