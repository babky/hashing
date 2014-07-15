#include "storages/collision_count_storage.h"
#include "systems/cwlf_system.h"
#include "systems/random_system.h"
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

template <typename Table>
void performTest(size_t runs, size_t m, size_t cells) {
	using namespace Hash;
	using namespace Hash::Storages;
	using namespace Hash::Systems;
	using namespace Hash::Utils;

	using namespace std;

	size_t max = 0, min = m, sum = 0;

	Table t;
	t.reserve(m);

	// Run it.
	for (size_t run = 0; run < runs; ++run) {
		t.clear();
		for (size_t j = 0; j < cells; ++j) {
			for (size_t i = 0; i < m / cells; ++i) {
				t.insert(i + m * j);
			}
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
}

int main(int argc, const char ** argv) {
	// Definitions.
	using namespace Hash;
	using namespace Hash::Storages;
	using namespace Hash::Systems;
	using namespace Hash::Utils;

	using namespace std;

	using namespace boost;
	using namespace boost::program_options;

	typedef Table<size_t, EqualityComparer<size_t>, RandomBin, CollisionCountStorage> HashTableRandom;
	typedef Table<size_t, EqualityComparer<size_t>, UniversalFunctionCWLF, CollisionCountStorage> HashTableCWLF;

	// Parse the command line.
	const size_t DEFAULT_M = 65536;
	const size_t DEFAULT_RUNS = 64;
	const size_t DEFAULT_CELLS = 1;
	const string DEFAULT_FUNCTION = "cwlf";

	size_t m = DEFAULT_M;
	size_t cells = DEFAULT_CELLS;
	string functionType = DEFAULT_FUNCTION;
	size_t runs = DEFAULT_RUNS;

	options_description optsDesc("Table M Test allowed options.");
	optsDesc.add_options()
		("help", "prints this help message")
		("m", value<size_t>(&m)->default_value(DEFAULT_M), "The m.")
		("cells", value<size_t>(&cells)->default_value(DEFAULT_CELLS), "The number of cells into which the input elements are split.")
		("runs", value<size_t>(&runs)->default_value(DEFAULT_RUNS), "The number of runs.")
		("function", value<string>(&functionType)->default_value(DEFAULT_FUNCTION), "Default function is a randomly chosen function from CWLF. Possible values:\n\tcwlf: a CWLF function,\n\trandom: a random function.");

	variables_map vm;
	store(parse_command_line(argc, argv, optsDesc), vm);
	notify(vm);

	if (vm.count("help")) {
		cout << optsDesc;
		return 0;
	}

	if (functionType == "cwlf") {
		performTest<HashTableCWLF>(runs, m, cells);
	} else if (functionType == "random") {
		performTest<HashTableRandom>(runs, m, cells);
	} else {
		cerr << "What the function? Use random or cwlf." << endl;
		return 1;
	}

	return 0;
}
