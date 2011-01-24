#ifndef CHAIN_LENGTH_AWARE_STORAGE_INFO_H
#define CHAIN_LENGTH_AWARE_STORAGE_INFO_H

#include "storage.h"

namespace Hash { namespace Utils {

	class ChainLengthAwareStorageInfo : public StorageInfo {
	public:
		virtual size_t getChainLength(size_t address) const = 0;
	};

} }

#endif /* CHAIN_LENGTH_AWARE_STORAGE_INFO_H */

