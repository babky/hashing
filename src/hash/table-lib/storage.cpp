#include "storage.h"

using namespace Hash;

const size_t StorageParams::INITIAL_STORAGE_SIZE = 16;

PlainStorageInfo::PlainStorageInfo(size_t aTableSize):
  tableSize(aTableSize),
  elementCount(0)
{
}

size_t PlainStorageInfo::getElementCount(void) const {
	return elementCount;
}

size_t PlainStorageInfo::getTableSize(void) const {
	return tableSize;
}

double PlainStorageInfo::getLoadFactor(void) const {
	return (double) elementCount / (double) tableSize;
}

MaxChainLengthStorageInfo::MaxChainLengthStorageInfo(size_t aTableSize):
  PlainStorageInfo(aTableSize),
  maxChainLength(0)
{
}

size_t MaxChainLengthStorageInfo::getMaxChainLength(void) const {
	return maxChainLength;
}

SettablePlainStorageInfo::SettablePlainStorageInfo(size_t aTableSize):
  SettableStorageInfo<PlainStorageInfo>(aTableSize) 
{
}

SettableMaxChainLengthStorageInfo::SettableMaxChainLengthStorageInfo(size_t aTableSize):
  SettableStorageInfo<MaxChainLengthStorageInfo>(aTableSize) 
{
}

void SettableMaxChainLengthStorageInfo::refineMaxChainLength(size_t refinement) {
	if (maxChainLength < refinement) {
		maxChainLength = refinement;
	}
}