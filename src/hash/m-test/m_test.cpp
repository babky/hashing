#include "storages/collision_count_storage.h"
#include "systems/cwlf_system.h"
#include "systems/random_system.h"
#include "table.h"
#include "utils/equality_comparer.h"
#include "utils/storage_statistics.h"
#include <iostream>
#include <boost/format.hpp>

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

template <typename base_t>
base_t compute_s_m_i(base_t s, size_t m, size_t i) {
	return Hash::Math::UnsignedDoubleWord<base_t>::add(
		i,
		Hash::Math::UnsignedDoubleWord<base_t>::multiply(s, m, Hash::Math::Prime<base_t>::GREATEST_PRIME),
		Hash::Math::Prime<base_t>::GREATEST_PRIME
	);
}

template <typename base_t, typename Table>
/**
 * @param f Multiplactive factor - number of children per one progression == length of the progression.
 * @param s Offset.
 */
void generate_data_cells(Table & t, size_t cell_height, size_t n, size_t m, size_t f, size_t cell_count, base_t s) {
	if (cell_height == 0) {
		for (size_t i = 0; i < n / cell_count; ++i) {
			// std::cout << compute_s_m_i(s, m, i) << " ";
			t.insert(compute_s_m_i<base_t>(s, m, i));
		}
	} else {
		for (size_t i = 0; i < f;  ++i) {
			generate_data(t, cell_height - 1, n, m, f, cell_count * f, compute_s_m_i<base_t>(s, m, i));
		}
	}
}

template <typename base_t, typename double_t, typename Table>
/**
 * @param f Multiplactive factor - number of children per one progression == length of the progression.
 * @param s Offset.
 */
void generate_data_many_collisions(Table & t, size_t cell_height, size_t n, size_t m, size_t f) {
	base_t p = Hash::Math::Prime<base_t>::GREATEST_PRIME;
	base_t r = 2;

	for (double_t j = 0; j < r; ++j) {
		for (double_t i = 0; i < n / r; ++i) {
			base_t el = (j * Hash::Math::Prime<base_t>::GREATEST_PRIME / 2 + i) % Hash::Math::Prime<base_t>::GREATEST_PRIME;
			if (t.contains(el)) {
				continue;
			}
			t.insert(el);
		}
	}

	/*
	f = 65536;
	base_t x1 = Hash::Math::Prime<base_t>::GREATEST_PRIME;
	// Divide the generated into the factors.
	// p/f, 2p/f, 3p/f, ...
	for (size_t i = 1; i < f; ++i) {
		base_t x = i * x1;

		for (size_t j = 0; j < n / f; ++j) {
			// Generate n / f elements.
			// For the first factor it is in the form:
			// factor = 0, fp/n, 2fp/n, ..., p
			// factor * p/f
			double_t factor = ((double_t) j * f * (Hash::Math::Prime<base_t>::GREATEST_PRIME / n));
			base_t el = (base_t) ((factor * x) % Hash::Math::Prime<base_t>::GREATEST_PRIME);
			if (t.contains(el)) {
				continue;
			}
			t.insert(el);
		}
	}
	/* */

	/*
	size_t r = 2 * exp2(log2(m) / 4);
	double_t p3 = Hash::Math::Prime<base_t>::GREATEST_PRIME / 3 ;
	for (size_t i = 0; i < r; ++i) {
		for (size_t j = 0; j < r; ++j) {
			for (size_t k = 0; k < r; ++k) {
				for (size_t l = 0; l < r; ++l) {
					size_t x = (((i) * (p3 + j) * (p3 + k) * (p3 + l)) % Hash::Math::Prime<base_t>::GREATEST_PRIME);
					if (t.contains(x)) {
						continue;
					}

					t.insert(x);
				}
			}
		}
	}
	*/

	/*
	for (size_t i = 1; i <= f; ++i) {
		for (size_t j = 0; j < n / f; ++j) {
			base_t el = (base_t) ((((double_t) i) * j) % Hash::Math::Prime<base_t>::GREATEST_PRIME);
			if (t.contains(el)) {
				continue;
			}

			t.insert(el);
		}
	}
	*/
}

template <typename base_t>
class verify_set {
public:
	bool contains(base_t x) {
		return s.find(x) != s.end();
	}

	void insert(base_t x) {
		if (contains(x)) {
			throw std::logic_error((boost::format("Duplicate key %1%") % x).str());
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

template<typename base_t, class Table>
class insert_to_counting_table {
public:
	insert_to_counting_table(Table & table):
		t(table) {
	}

	bool contains(base_t x) {
		return verifier.contains(x);
	}

	void insert(base_t x) {
		verifier.insert(x);
		t.insert(x);
	}

	void print() const {
		verifier.print();
	}

private:
	verify_set<base_t> verifier;
	Table & t;
};


size_t compute_hierarchy_factor(size_t n) {
	return exp2((size_t) (log2(n) / log2(log2(n))));
}

template <typename base_t, typename double_t, typename Table>
void performTest(size_t runs, size_t n, size_t m, size_t cell_height, bool print_set) {
	using namespace Hash;
	using namespace Hash::Storages;
	using namespace Hash::Systems;
	using namespace Hash::Utils;

	using namespace std;

	size_t max = 0, min = m, sum = 0;

	Table t;
	t.reserve(m);

	verify_set<base_t> s;
	// generate_data_many_collisions<base_t, double_t, Table>(t, cell_height, n, m, compute_hierarchy_factor(n), 1, 0);
	generate_data_many_collisions<base_t, double_t, verify_set<base_t>>(s, cell_height, n, m, 32);
	if (print_set) {
		s.print();
	}

	// Run it.
	for (size_t run = 0; run < runs; ++run) {
		t.clear();
		insert_to_counting_table<base_t, Table> ins(t);
		generate_data_many_collisions<base_t, double_t, insert_to_counting_table<base_t, Table>>(ins, cell_height, n, m, 32);

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

#ifdef __GNUC__
	#ifdef __LP64__
		typedef uint64_t base_t;
		typedef __uint128_t double_t;
	#else
		typedef uint32_t base_t;
		typedef uint64_t double_t;
	#endif
#else
	typedef uint64_t base_t;
	typedef uint128_t double_t;
#endif

	// Parse the command line.
	const size_t DEFAULT_M = 4096;
	const size_t DEFAULT_N = 0;
	const size_t DEFAULT_RUNS = 64;
	const size_t DEFAULT_CELL_HEIGHT = 0;
	const string DEFAULT_FUNCTION = "cwlf";
	const bool DEFAULT_PRINT_SET = false;

	size_t m = DEFAULT_M;
	size_t n = DEFAULT_N;
	size_t cell_height = DEFAULT_CELL_HEIGHT;
	string functionType = DEFAULT_FUNCTION;
	size_t runs = DEFAULT_RUNS;
	bool print_set = DEFAULT_PRINT_SET;

	options_description optsDesc("Table M Test allowed options.");
	optsDesc.add_options()
		("help", "prints this help message")
		("m", value<size_t>(&m)->default_value(DEFAULT_M), "The size of the table.")
		("n", value<size_t>(&n)->default_value(DEFAULT_N), "The number of elements. When zero is given we choose n = m.")
		("cell_height", value<size_t>(&cell_height)->default_value(DEFAULT_CELL_HEIGHT), "The height of the cell hierarchy into which the input elements are split.")
		("runs", value<size_t>(&runs)->default_value(DEFAULT_RUNS), "The number of runs.")
		("print_set", value<bool>(&print_set)->default_value(DEFAULT_PRINT_SET), "If the hashed set should be printed out.")
		("function", value<string>(&functionType)->default_value(DEFAULT_FUNCTION), "Default function is a randomly chosen function from CWLF. Possible values:\n\tcwlf: a CWLF function,\n\trandom: a random function.");

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

	try {
		if (n == 0) {
			n = m;
		}

		typedef Table<base_t, EqualityComparer<base_t>, RandomBin, CollisionCountStorage> HashTableRandom;
		typedef Table<base_t, EqualityComparer<base_t>, UniversalFunctionCWLF, CollisionCountStorage> HashTableCWLF;

		if (functionType == "cwlf") {
			performTest<base_t, double_t, HashTableCWLF>(runs, n, m, cell_height, print_set);
		} else if (functionType == "random") {
			performTest<base_t, double_t, HashTableRandom>(runs, n, m, cell_height, print_set);
		} else {
			cerr << "What the function? Use random or CWLF." << endl;
			return 1;
		}
	} catch (std::exception & e) {
		std::cerr << e.what() << std::endl;
		throw e;
	}

	return 0;
}
