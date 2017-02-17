#include "systems/bad_linear_system.h"
#include "storages/chained_storage.h"
#include "utils/equality_comparer.h"
#include <iostream>

using namespace Hash::Storages;
using namespace Hash::Systems;
using namespace Hash::Utils;
using namespace std;

int main(int argc, char ** argv) {
	typedef ChainedStorage<size_t, EqualityComparer<size_t>, size_t> Storage;
	BadLinearSystem<size_t, Storage> bls(17);
	size_t out = bls.hash((16 << 5) | 16);
	return 0;
}
