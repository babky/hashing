#include "storage_statistics.h"

using namespace Hash::Utils;

StorageStatistics::StorageStatistics(void):
  elementCount(0),
  tableLength(0),
  minChainLength(0),  
  maxChainLength(0) {
}

size_t StorageStatistics::getElementCount(void) const {
	return this->elementCount;
}

size_t StorageStatistics::getTableLength(void) const {
	return this->tableLength;
}

double StorageStatistics::getLoadFactor(void) const {
	if (this->tableLength == 0) {
		return 0;
	} else {
		return static_cast<double>(this->elementCount) / this->tableLength;
	}
}

size_t StorageStatistics::getMinimalChainLength(void) const {
	return this->minChainLength;
}

size_t StorageStatistics::getMaximalChainLength(void) const {
	return this->maxChainLength;
}

void StorageStatistics::addChain(size_t chainLength) {
	if (this->minChainLength > chainLength) {
		this->minChainLength = chainLength;
	}

	if (this->maxChainLength < chainLength) {
		this->maxChainLength = chainLength;
	}

	++this->tableLength;
	this->elementCount += chainLength;
}

void StorageStatistics::clear(void) {
	this->maxChainLength = 0;
	this->minChainLength = 0;
	this->elementCount = 0;
	this->tableLength = 0;
}
