#include "worst_case_rehash_policy.h"

using namespace Hash;
using namespace Hash::Policies;
using namespace Hash::Utils;
using namespace Hash::Policies::Rehash;

const double WorstCaseRehashPolicy::DEFAULT_MIN_LOAD_FACTOR = 0.75;
const double WorstCaseRehashPolicy::DEFAULT_MAX_LOAD_FACTOR = 0.95;
const size_t WorstCaseRehashPolicy::ELPSL_FACTOR = 4;
const size_t WorstCaseRehashPolicy::ELPSL_CONSTANT = 2420;

WorstCaseRehashPolicy::WorstCaseRehashPolicy(double minFactor, double maxFactor):
  minFactor(minFactor),
  maxFactor(maxFactor) 
{ }

double WorstCaseRehashPolicy::getMinLoadFactor(void) const {
	return this->minFactor;
}

double WorstCaseRehashPolicy::getMaxLoadFactor(void) const {
	return this->maxFactor;
}

bool WorstCaseRehashPolicy::needsRehashingAfterInsert(const StorageInfo & storageInfo) {
	StorageStatistics stats;
	storageInfo.computeStatistics(stats);
	size_t maxLength = ELPSL_FACTOR * ELPSL_CONSTANT * stats.getTableLength();

	return storageInfo.getLoadFactor() > this->getMaxLoadFactor() || stats.getMaximalChainLength() > maxLength;
}

bool WorstCaseRehashPolicy::needsRehashingAfterDelete(const StorageInfo & storageInfo) {
	return storageInfo.getLoadFactor() < this->getMinLoadFactor();
}
