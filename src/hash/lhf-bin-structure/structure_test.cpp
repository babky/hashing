#include "storages/chained_storage.h"
#include "systems/linear_map_system.h"
#include "table.h"
#include "utils/equality_comparer.h"
#include "utils/storage_statistics.h"
#include <iostream>
#include <boost/format.hpp>
#include <limits>

/**
 * This test is used to compute the size of the largest bin for various hash systems.
 */

#ifdef BOOST_MSVC
#pragma warning(disable: 4512)
#endif
#include <boost/program_options.hpp>
#ifdef BOOST_MSVC
#pragma warning(default: 4512)
#endif

using namespace std;

template<typename Table>
void generate_subspace(Table & t, size_t m) {
    size_t x;
    size_t threshold = m / 256;
    if (!threshold) {
        threshold = 1;
    }
	for (size_t i = 0; i < m; ++i) {
        x = (i / threshold) * threshold * 16 + (i % threshold);
        x *= 16;
        t.insert(x);
	}
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

	// Parse the command line.
	const size_t DEFAULT_M = 4096;
	const size_t DEFAULT_RUNS = 1024;

	size_t m = DEFAULT_M;
	size_t runs = DEFAULT_RUNS;

	options_description optsDesc("Table M Test allowed options.");
	optsDesc.add_options()
		("help,h", "Prints this help message.")
		("table-size,m", value<size_t>(&m)->default_value(DEFAULT_M), "The size of the table.")
		("runs,r", value<size_t>(&runs)->default_value(DEFAULT_RUNS), "The number of runs.");

	variables_map vm;
	try {
		store(parse_command_line(argc, argv, optsDesc), vm);
		notify(vm);
	} catch (std::exception & e) {
		std::cerr << e.what() << std::endl;
		throw e;
	}

	if (vm.count("help")) {
		cout << optsDesc;
		return 0;
	}

	typedef Table<size_t, EqualityComparer<size_t>, UniversalFunctionLinearMap, ChainedStorage> HashTableLinearMap;
	HashTableLinearMap t;
	StorageStatistics st;

    size_t sum = 0;

	for (size_t i = 0; i < runs; ++i) {
		t.clear();
		st.clear();
		generate_subspace(t, m);
		t.computeStatistics(st);

		if (st.getMaxChainLength() != st.getMinChainLength()) {
			cerr << "Bin structure assumption failure.\n";
			return 1;
		}

        sum += st.getMaxChainLength();
	}

	cout << "Bin structure assumption succeeded after " << runs << " runs for " << m << " elements.\n";
    cout << "E lbin = " << static_cast<double>(sum) / runs << "\n";

	return 0;
}
