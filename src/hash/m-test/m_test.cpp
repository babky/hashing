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

template <typename base_t>
base_t compute_s_m_i(base_t s, size_t m, size_t i) {
	return Hash::Math::UnsignedDoubleWord<base_t>::add(
		i,
		Hash::Math::UnsignedDoubleWord<base_t>::multiply(s, m, Hash::Math::Prime<base_t>::GREATEST_PRIME),
		Hash::Math::Prime<base_t>::GREATEST_PRIME
	);
}

template <typename base_t, typename Table>
void generate_data(Table & t, size_t cell_height, size_t m, size_t f, size_t cell_count, base_t s) {
	if (cell_height == 0) {
		for (size_t i = 0; i < m / cell_count; ++i) {
			// std::cout << compute_s_m_i(s, m, i) << " ";
			t.insert(compute_s_m_i<base_t>(s, m, i));
		}
	} else {
		for (size_t i = 0; i < f;  ++i) {
			generate_data(t, cell_height - 1, m, f, cell_count * f, compute_s_m_i<base_t>(s, m, i));
		}
	}
}

template <typename base_t>
class verify_set {
public:
	void insert(base_t x) {
		if (s.find(x) != s.end()) {
			throw std::logic_error("Duplicate key.");
		}

		s.insert(x);
	}

	void print() const {
		for (typename set_type::const_iterator it = s.begin(); it != s.end(); ++it) {
			std::cout << *(it) << "\n";
		}
	}

private:
	typedef std::set<base_t> set_type;
	set_type s;
};

template <typename base_t, typename Table>
void performTest(size_t runs, size_t m, size_t cell_height, bool print_set) {
	using namespace Hash;
	using namespace Hash::Storages;
	using namespace Hash::Systems;
	using namespace Hash::Utils;

	using namespace std;

	size_t max = 0, min = m, sum = 0;

	Table t;
	t.reserve(m);

	verify_set<base_t> s;
	generate_data<base_t, verify_set<base_t>>(s, cell_height, m, (size_t) exp2(log2(m) / log(log2(m))) - 1, 1, 0);
	if (print_set) {
		s.print();
	}

	// Run it.
	for (size_t run = 0; run < runs; ++run) {
		t.clear();
		generate_data<base_t, Table>(t, cell_height, m, (size_t) exp2(log2(m) / log(log2(m))) - 1, 1, 0);

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

	// Parse the command line.
	const size_t DEFAULT_M = 65536;
	const size_t DEFAULT_RUNS = 64;
	const size_t DEFAULT_CELL_HEIGHT = 0;
	const string DEFAULT_FUNCTION = "cwlf";
	const bool DEFAULT_PRINT_SET = false;

	size_t m = DEFAULT_M;
	size_t cell_height = DEFAULT_CELL_HEIGHT;
	string functionType = DEFAULT_FUNCTION;
	size_t runs = DEFAULT_RUNS;
	bool print_set = DEFAULT_PRINT_SET;

	options_description optsDesc("Table M Test allowed options.");
	optsDesc.add_options()
		("help", "prints this help message")
		("m", value<size_t>(&m)->default_value(DEFAULT_M), "The m.")
		("cell_height", value<size_t>(&cell_height)->default_value(DEFAULT_CELL_HEIGHT), "The height of the cell hierarchy into which the input elements are split.")
		("runs", value<size_t>(&runs)->default_value(DEFAULT_RUNS), "The number of runs.")
		("print_set", value<bool>(&print_set)->default_value(DEFAULT_PRINT_SET), "If the hashed set should be printed out.")
		("function", value<string>(&functionType)->default_value(DEFAULT_FUNCTION), "Default function is a randomly chosen function from CWLF. Possible values:\n\tcwlf: a CWLF function,\n\trandom: a random function.");

	variables_map vm;
	store(parse_command_line(argc, argv, optsDesc), vm);
	notify(vm);

	if (vm.count("help")) {
		cout << optsDesc;
		return 0;
	}

	try {
#ifdef __GNUC__
	#ifdef __LP64__
		typedef __uint128_t base_t;
	#else
		typedef uint64_t base_t;
	#endif
#else
	typedef uint64_t base_t;
#endif
		typedef Table<base_t, EqualityComparer<base_t>, RandomBin, CollisionCountStorage> HashTableRandom;
		typedef Table<base_t, EqualityComparer<base_t>, UniversalFunctionCWLF, CollisionCountStorage> HashTableCWLF;

		if (functionType == "cwlf") {
			performTest<base_t, HashTableCWLF>(runs, m, cell_height, print_set);
		} else if (functionType == "random") {
			performTest<base_t, HashTableRandom>(runs, m, cell_height, print_set);
		} else {
			cerr << "What the function? Use random or cwlf." << endl;
			return 1;
		}
	} catch (std::exception & e) {
		std::cerr << e.what() << std::endl;
		throw e;
	}

	return 0;
}
