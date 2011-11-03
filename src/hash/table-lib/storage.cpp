#include "storage.h"

using namespace Hash;

const size_t StorageParams::INITIAL_STORAGE_SIZE = 16;

PlainStorageInfo::PlainStorageInfo(size_t aTableSize):
  tableSize(aTableSize),
  elementCount(0)
{
}

MaxChainLengthStorageInfo::MaxChainLengthStorageInfo(size_t aTableSize):
  PlainStorageInfo(aTableSize),
  maxChainLength(0)
{
}

SettablePlainStorageInfo::SettablePlainStorageInfo(size_t aTableSize):
  SettableStorageInfo<PlainStorageInfo>(aTableSize) 
{
}

SettableMaxChainLengthStorageInfo::SettableMaxChainLengthStorageInfo(size_t aTableSize):
  SettableStorageInfo<MaxChainLengthStorageInfo>(aTableSize) 
{
}

