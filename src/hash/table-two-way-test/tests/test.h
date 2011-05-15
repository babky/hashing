#ifndef TWT_TEST_H
#define TWT_TEST_H

#include "utils/storage_statistics.h"

namespace Hash { namespace Tests {

	class Test {
	public:
		virtual ~Test(void);
		virtual void run(void) = 0;
		virtual void computeStatistics(Hash::Utils::StorageStatistics & stats) = 0;
	};

} }

#endif /* TWT_TEST_H */
