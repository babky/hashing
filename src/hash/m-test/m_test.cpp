#include "storages/chained_storage.h"
#include "systems/cwlf_system.h"
#include "table.h"
#include "utils/equality_comparer.h"
#include "utils/storage_statistics.h"
#include <iostream>

#ifdef BOOST_MSVC
	#pragma warning(disable: 4512)
#endif
#include <boost/program_options.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512)
#endif


int main(int argc, const char ** argv) {
	// Definitions.
	using namespace Hash;
	using namespace Hash::Storages;
	using namespace Hash::Systems;
	using namespace Hash::Utils;

	using namespace std;

	using namespace boost;
	using namespace boost::program_options;

	typedef Table<size_t, EqualityComparer<size_t>, UniversalFunctionCWLF, ChainedStorage> HashTable;

	// Parse the command line.
	const size_t DEFAULT_M = 65536;
	const size_t DEFAULT_RUNS = 64;

	size_t m = DEFAULT_M;
	size_t runs = DEFAULT_RUNS;

	options_description optsDesc("Table Two Way Test allowed options.");
	optsDesc.add_options()
		("help", "prints this help message")
		("m", value<size_t>(&m)->default_value(DEFAULT_M), "The m.")
		("runs", value<size_t>(&runs)->default_value(DEFAULT_RUNS), "The number of runs.");

	variables_map vm;
	store(parse_command_line(argc, argv, optsDesc), vm);
	notify(vm);

	if (vm.count("help")) {
		cout << optsDesc;
		return 0;
	}

	size_t max = 0, min = m, sum = 0;

	// Run it.
	for (size_t run = 0; run < runs; ++run) {
		HashTable t;
		for (size_t i = 0; i < m; ++i) {
			t.insert(i);
		}

		StorageStatistics stats;
		t.computeStatistics(stats);
		cout << stats << endl;

		if (max < stats.getMaxChainLength()) {
			max = stats.getMaxChainLength();
		}

		if (min > stats.getMaxChainLength()) {
			min = stats.getMaxChainLength();
		}

		sum += stats.getMaxChainLength();
	}

	cout << "MIN:\t" << min << "\n";
	cout << "AVG:\t" << ((double) sum) / runs << "\n";
	cout << "MAX:\t" << max << endl;

	return 0;
}
