#include "storage_statistics.h"
#include <limits>

using namespace Hash::Utils;

StorageStatistics::StorageStatistics(void):
  elementCount(0),
  tableLength(0),
  minChainLength(std::numeric_limits<size_t>::max()),
  maxChainLength(0),
  rehashCount(0) {
}

size_t StorageStatistics::getElementCount(void) const {
	return this->elementCount;
}

size_t StorageStatistics::getTableLength(void) const {
	return this->tableLength;
}

void StorageStatistics::setTableLength(size_t length) {
	tableLength = length;
}

double StorageStatistics::getLoadFactor(void) const {
	if (this->tableLength == 0) {
		return 0;
	} else {
		return static_cast<double>(this->elementCount) / this->tableLength;
	}
}

size_t StorageStatistics::getMinChainLength(void) const {
	return this->minChainLength;
}

size_t StorageStatistics::getMaxChainLength(void) const {
	return this->maxChainLength;
}

void StorageStatistics::addChain(size_t chainLength) {
	if (chainLength && this->minChainLength > chainLength) {
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
	this->minChainLength = std::numeric_limits<size_t>::max();
	this->elementCount = 0;
	this->tableLength = 0;
	this->rehashCount = 0;
}

void StorageStatistics::rehash(void) {
	++this->rehashCount;
}

size_t StorageStatistics::getRehashCount(void) const {
	return this->rehashCount;
}

std::ostream & Hash::Utils::operator <<(std::ostream & out, const StorageStatistics & stats) {
	out << "Element count: " << stats.getElementCount() << "\n";
	out << "Table length:  " << stats.getTableLength() << "\n";
	out << "Load factor:   " << stats.getLoadFactor() << "\n";
	out << "Min chain:     " << stats.getMinChainLength() << "\n";
	out << "Max chain:     " << stats.getMaxChainLength() << "\n";
	out << "Rehash count:  " << stats.getRehashCount() << "\n";

	return out;
}
