#include "rehash_observer.h"
#include <iostream>

using namespace std;
using namespace Hash;
using namespace Hash::Utils;

RehashObserver::~RehashObserver(void) {
}

void DummyRehashObserver::rehash(StorageInfo &) {
}

void PrintingRehashObserver::rehash(StorageInfo & info) {
	std::cout << "Rehash when the table has the size of " << info.getTableSize() << " elements.\n";
}
