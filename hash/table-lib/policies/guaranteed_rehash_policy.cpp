#include "guaranteed_rehash_policy.h"
#include <cmath>

using namespace Hash;
using namespace Hash::Policies;
using namespace Hash::Policies::Rehash;
using namespace Hash::Utils;
using namespace std;

double log2(double n) {
	return log(static_cast<double> (n)) / log(static_cast<double> (2));
}

GuaranteedRehashPolicy::GuaranteedRehashPolicy(bool allowDelete):
  allowDelete(allowDelete),
  maxLoadFactor(allowDelete ? MAX_LOAD_FACTOR_ALLOW_DELETE : MAX_LOAD_FACTOR_NO_DELETE), 
  minLoadFactor(allowDelete ? MIN_LOAD_FACTOR_ALLOW_DELETE : MIN_LOAD_FACTOR_NO_DELETE),
  multiplicativeConstant(allowDelete ? MULTIPLICATIVE_CONSTANT_ALLOW_DELETE : MULTIPLICATIVE_CONSTANT_NO_DELETE) {
}

double GuaranteedRehashPolicy::getMinLoadFactor(void) const {
	return this->minLoadFactor;
}

double GuaranteedRehashPolicy::getMaxLoadFactor(void) const {
	return this->maxLoadFactor;
}

size_t GuaranteedRehashPolicy::getMaxChainLength(size_t n) const {
	double t = log2(n);
    size_t maxLength = static_cast<size_t> (ceil(this->multiplicativeConstant * t * log2(t)));
	if (maxLength < MINIMAL_ALLOWED_BOUND) {
		return MINIMAL_ALLOWED_BOUND;
	} else {
		return maxLength;
	}
}

inline bool GuaranteedRehashPolicy::needsRehashingAfterDelete(const Hash::StorageInfo & storageInfo) {
	if (storageInfo.getLoadFactor() < this->getMinLoadFactor()) {
		return true;
	}

	return false;
}

inline bool GuaranteedRehashPolicy::needsRehashingAfterInsert(const Hash::StorageInfo & storageInfo) {
	if (storageInfo.getLoadFactor() > this->getMaxLoadFactor()) {
		return true;
	}

	StorageStatistics stats;
	storageInfo.computeStatistics(stats);
	if (stats.getMaximalChainLength() > this->getMaxChainLength(stats.getElementCount())) {
		return true;
	}

	return false;
}

const double GuaranteedRehashPolicy::MIN_LOAD_FACTOR_NO_DELETE = 0.5;
const double GuaranteedRehashPolicy::MAX_LOAD_FACTOR_NO_DELETE = 1;
const double GuaranteedRehashPolicy::MULTIPLICATIVE_CONSTANT_NO_DELETE = 19.152;

const double GuaranteedRehashPolicy::MIN_LOAD_FACTOR_ALLOW_DELETE = 0.25;
const double GuaranteedRehashPolicy::MAX_LOAD_FACTOR_ALLOW_DELETE = 1;
const double GuaranteedRehashPolicy::MULTIPLICATIVE_CONSTANT_ALLOW_DELETE = 28.728;

const size_t GuaranteedRehashPolicy::MINIMAL_ALLOWED_BOUND = 512;
