#include <iostream>
#include <vector>
#include <numeric>
#include <utility>
#include "boost/multiprecision/cpp_int.hpp"
#include "systems/cwlf_system.h"
#include "systems/multiply_shift_system.h"
#include "storages/chained_storage.h"
#include "utils/equality_comparer.h"
#include "table.h"
#ifdef BOOST_MSVC
#pragma warning(disable: 4512)
#endif
#include <boost/program_options.hpp>
#ifdef BOOST_MSVC
#pragma warning(default: 4512)
#endif

using namespace std;
using namespace Hash;
using namespace Hash::Systems;

typedef vector<size_t> ElementVector;

ElementVector first(size_t count) {
	ElementVector v;
	v.reserve(count);

	for (size_t i = 0; i < count; ++i) {
		v.push_back(i);
	}

	return v;
}

ostream & operator <<(ostream & out, const ElementVector & v) {
	for (size_t i = 0; i < v.size(); ++i) {
		if (i > 0) {
			out << " ";
		}
		out << v[i];
	}
	return out;
}

bool next(ElementVector & v, size_t max) {
	size_t s = v.size() - 1;

	while (v[s] == max) {
		// First two elements may be fixed to 0 and 1 for linear functions.
		if (s == 2) {
			return false;
		}

		--s;
		--max;
	}

	for (++v[s], ++s; s < v.size(); ++s) {
		v[s] = v[s - 1] + 1;
	}

	return true;
}

template<class Function>
bool is_surjective(const ElementVector & v, Function f, bool * table) {
	for (size_t i = 0; i < f.getTableSize(); ++i) {
		table[i] = false;
	}

	for (size_t i = 0; i < v.size(); ++i) {
		table[f.hash(v[i])] = true;
	}

	return accumulate(table, table + f.getTableSize(), true, [](bool a, bool b) {return a & b;});
}

struct SurjectivityResult {

	SurjectivityResult(size_t aSurjectiveCount, size_t aFunctionCount) :
			surjectiveCount(aSurjectiveCount), functionCount(aFunctionCount) {
	}

	size_t surjectiveCount;
	size_t functionCount;

};

template<class Function, class FunctionGenerator>
SurjectivityResult surjective_number(const ElementVector & v, FunctionGenerator g) {
	size_t count = 0;
	size_t surjectiveCount = 0;
	bool * table = 0;
	Function f = g.next();
	table = new bool[f.getTableSize()];
	for (;; f = g.next()) {
		++count;
		if (is_surjective(v, f, table)) {
			++surjectiveCount;
		}

		if (!g.hasNext()) {
			break;
		}
	}
	delete[] table;
	table = 0;

	return SurjectivityResult(surjectiveCount, count);
}

struct LongestChainResult {

	LongestChainResult(void) :
			sum(0), count(0) {
	}

	LongestChainResult(size_t aSum, size_t aFunctionCount) :
			sum(aSum), count(aFunctionCount) {
	}

	size_t sum;
	size_t count;

};

typedef Table<size_t, Hash::Utils::EqualityComparer<size_t>, UniversalFunctionCWLF, Hash::Storages::ChainedStorage> TableCWLF;
typedef Table<size_t, Hash::Utils::EqualityComparer<size_t>, MultiplyShiftSystem, Hash::Storages::ChainedStorage> TableMultiplyShift;

template<class Table, class Function, class FunctionGenerator>
LongestChainResult longest_chain(const ElementVector & v, FunctionGenerator g) {
	size_t count = 0;
	size_t sum = 0;

	Function f = g.next();
	Hash::Utils::StorageStatistics stats;
	for (;; f = g.next()) {
		Table table(f);
		stats.clear();

		for (size_t i = 0; i < v.size(); ++i) {
			table.insert(v[i]);
		}

		table.computeStatistics(stats);
		sum += stats.getMaxChainLength();
		++count;

		if (!g.hasNext()) {
			break;
		}
	}

	return LongestChainResult(sum, count);
}

template<class HashFunction>
struct GeneratorFactoryTraits {

	typedef typename HashFunction::Generator Generator;

	static Generator create_generator(size_t universeSize, size_t tableSize) {
		return Generator(universeSize, tableSize);
	}

};

template<typename T, class Storage>
struct GeneratorFactoryTraits<MultiplyShiftSystem<T, Storage>> {

	typedef typename MultiplyShiftSystem<T, Storage>::Generator Generator;

	static Generator create_generator(size_t universeSize, size_t tableSize) {
		return Generator(universeSize - 1, tableSize);
	}

};

template<class Table, bool intermediate>
void perform_experiment(size_t universeSize, size_t setSize, size_t tableSize) {
	ElementVector v = first(setSize);
	SurjectivityResult surjectivityResult(0, 0);
	LongestChainResult longestChainResult;

	bool first = true;
	SurjectivityResult sResult(0, 0);
	ElementVector sVector;
	LongestChainResult lResult;
	ElementVector lVector;

	for (bool shouldContinue = true; shouldContinue; shouldContinue = next(v, universeSize - 1)) {
		typedef typename Table::HashFunction::Generator Generator;
		typedef typename Table::HashFunction HashFunction;

		Generator gs = GeneratorFactoryTraits<HashFunction>::create_generator(universeSize, tableSize);
		surjectivityResult = surjective_number<HashFunction, Generator>(v, gs);

		Generator gl = GeneratorFactoryTraits<HashFunction>::create_generator(universeSize, setSize);
		longestChainResult = longest_chain<Table, HashFunction, Generator>(v, gl);

		if (intermediate) {
			cout << v << " " << surjectivityResult.surjectiveCount << "/" << surjectivityResult.functionCount << " "
					<< longestChainResult.sum << "/" << longestChainResult.count << "\n";
		}

		if (first) {
			first = false;
			sVector = v;
			sResult = surjectivityResult;

			lVector = v;
			lResult = longestChainResult;
		} else {
			if (surjectivityResult.surjectiveCount < sResult.surjectiveCount) {
				sVector = v;
				sResult = surjectivityResult;
			}

			if (longestChainResult.sum > lResult.sum) {
				lVector = v;
				lResult = longestChainResult;
			}
		}
	}

	cout << "Results\n";
	cout << "Surjectivity:  " << sVector << " " << sResult.surjectiveCount << "/" << sResult.functionCount << "\n";
	cout << "Longest chain: " << lVector << " " << lResult.sum << "/" << lResult.count << " "
			<< boost::multiprecision::cpp_rational(lResult.sum, lResult.count).convert_to<double>() << "\n";
}

template<class Table, bool intermediate>
void perform_partial_experiment(size_t universeSize, const ElementVector & set, size_t tableSize) {
	typedef typename Table::HashFunction::Generator Generator;
	typedef typename Table::HashFunction HashFunction;

	Generator gs = GeneratorFactoryTraits<HashFunction>::create_generator(universeSize, tableSize);
	SurjectivityResult sResult = surjective_number<HashFunction, Generator>(set, gs);

	Generator gl = GeneratorFactoryTraits<HashFunction>::create_generator(universeSize, Hash::Math::next_power(set.size()));
	LongestChainResult lResult = longest_chain<Table, HashFunction, Generator>(set, gl);

	cout << "Results\n";
	cout << "Surjectivity:  " << set << " " << sResult.surjectiveCount << "/" << sResult.functionCount << "\n";
	cout << "Longest chain: " << set << " " << lResult.sum << "/" << lResult.count << " "
			<< boost::multiprecision::cpp_rational(lResult.sum, lResult.count).convert_to<double>() << "\n";
}

/**
 * Performs the surjectivity and longest chain experiment for a single set.
 * The set is in the form {0, 1, ..., log(tableSize)}.
 */
template<class Table, bool intermediate>
void perform_partial_experiment(size_t universeSize, size_t setSize, size_t tableSize) {
	ElementVector set;

	for (size_t i = 0; i < setSize; ++i) {
		set.push_back(i);
	}

	perform_partial_experiment<Table, intermediate>(universeSize, set, tableSize);
}

template<class Table, bool intermediate>
void perform_partial_experiment_log_set(size_t universeSize, size_t setSize, size_t tableSize) {
	ElementVector set;

	size_t log = Hash::Math::log2exact(tableSize);
	for (size_t i = 0; i < log; ++i) {
		set.push_back(i);
	}

	for (size_t x = tableSize; set.size() < setSize; x += tableSize) {
		set.push_back(x);
	}

	perform_partial_experiment<Table, intermediate>(universeSize, set, tableSize);
}

int main(int argc, char ** argv) {
	using namespace boost::program_options;

	size_t universeSize = 17;
	size_t setSize = 8;
	size_t tableSize = 4;
	bool intermediate = false;
	bool shortExperiment = false;
	string function = "cwlf";

	options_description optsDesc("Probability distribution computation.");
	optsDesc.add_options()\
		("help,h", "prints this help message")\
		("universe,u", value<size_t>(&universeSize)->default_value(universeSize), "Universe")\
		("set,s", value<size_t>(&setSize)->default_value(setSize), "The size of the set.")\
		("table,t", value<size_t>(&tableSize)->default_value(tableSize), "The size of the table.")\
		("short-experiment,e", value<bool>(&shortExperiment)->default_value(shortExperiment), "Short experiment.")\
		("function,f", value<string>(&function)->default_value(function), "Function type.")\
		("intermediate,i", value<bool>(&intermediate)->default_value(intermediate), "If the intermediate output should be provided.");

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

	if (function == "cwlf") {
		if (intermediate) {
			if (shortExperiment) {
				perform_partial_experiment_log_set<TableCWLF, true>(universeSize, setSize, tableSize);
			} else {
				perform_experiment<TableCWLF, true>(universeSize, setSize, tableSize);
			}
		} else {
			if (shortExperiment) {
				perform_partial_experiment_log_set<TableCWLF, true>(universeSize, setSize, tableSize);
			} else {
				perform_experiment<TableCWLF, false>(universeSize, setSize, tableSize);
			}
		}
	} else if (function == "multiply-shift") {
		if (!Hash::Math::is_power_of_2(setSize)) {
			cerr << "The set size must be power of 2 for multiply-shift.\n";
			return 1;
		}

		if (!Hash::Math::is_power_of_2(tableSize)) {
			cerr << "The table size size must be power of 2 for multiply-shift.\n";
			return 2;
		}


		if (intermediate) {
			if (shortExperiment) {
				perform_partial_experiment<TableMultiplyShift, true>(universeSize, setSize, tableSize);
			} else {
				perform_experiment<TableMultiplyShift, true>(universeSize, setSize, tableSize);
			}
		} else {
			if (shortExperiment) {
				perform_partial_experiment<TableMultiplyShift, true>(universeSize, setSize, tableSize);
			} else {
				cout << "Multiply-shift long experiment wihtout immediate results." << endl;
				perform_experiment<TableMultiplyShift, false>(universeSize, setSize, tableSize);
			}
		}
	}

	return 0;
}
