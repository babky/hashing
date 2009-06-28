#ifndef STORAGE_STATISTICS_H
#define STORAGE_STATISTICS_H

namespace Hash { namespace Utils {

	class StorageStatistics {
	public:
		StorageStatistics(void);

		size_t getElementCount(void) const;
		size_t getTableLength(void) const;
		double getLoadFactor(void) const;
		size_t getMinimalChainLength(void) const;
		size_t getMaximalChainLength(void) const;

		void addChain(size_t chainLength);
		void clear(void);

	private:
		size_t elementCount;
		size_t tableLength;
		size_t minChainLength;
		size_t maxChainLength;
	};

} }

#endif /* STORAGE_STATISTICS_H */