#include "rehash_policy.h"

using namespace Hash;
using namespace Hash::Policies;
using namespace Hash::Policies::Rehash;

const double LoadFactorBoundsRehashPolicy::DEFAULT_MIN_LOAD_FACTOR = 0.5;
const double LoadFactorBoundsRehashPolicy::DEFAULT_MAX_LOAD_FACTOR = 0.75;

LoadFactorBoundsRehashPolicy::LoadFactorBoundsRehashPolicy(double minFactor, double maxFactor):
  minFactor(minFactor),
  maxFactor(maxFactor) {
}

double LoadFactorBoundsRehashPolicy::getMinLoadFactor(void) const {
	return this->minFactor;
}

double LoadFactorBoundsRehashPolicy::getMaxLoadFactor(void) const {
	return this->maxFactor;
}

bool LoadFactorBoundsRehashPolicy::needsRehashingAfterInsert(const Hash::StorageInfo & storageInfo) {
	return storageInfo.getLoadFactor() > this->getMaxLoadFactor();
}

bool LoadFactorBoundsRehashPolicy::needsRehashingAfterDelete(const Hash::StorageInfo & storageInfo) {
	return storageInfo.getLoadFactor() < this->getMinLoadFactor();
}
