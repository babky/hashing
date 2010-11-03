#ifndef REHASH_OBSERVER
#define REHASH_OBSERVER

#include "../storage.h"

namespace Hash { namespace Utils {

	class RehashObserver {
	public:
		virtual ~RehashObserver(void);
		virtual void rehash(Hash::StorageInfo & info) = 0;
	};

	class DummyRehashObserver : public RehashObserver {
	public:
		virtual void rehash(Hash::StorageInfo & info);
	};

	class PrintingRehashObserver : public RehashObserver {
	public:
		virtual void rehash(Hash::StorageInfo & info);
	};

} }

#endif /* REHASH_OBSERVER */
