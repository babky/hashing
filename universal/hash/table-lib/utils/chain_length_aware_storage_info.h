#ifndef CHAIN_LENGTH_AWARE_STORAGE_INFO_H
#define CHAIN_LENGTH_AWARE_STORAGE_INFO_H

#include "storage.h"

namespace Hash { namespace Utils {

	/**
	 * Interface of the storage that stores the lengths of the chains.
	 */
	class ChainLengthAwareStorageInfo : public StorageInfo {
	public:
		/**
		 * Length of the chain at the given address.
		 *
		 * @param address The address of the chain.
		 */
		virtual size_t getChainLength(size_t address) const = 0;
	};

} }

#endif /* CHAIN_LENGTH_AWARE_STORAGE_INFO_H */

