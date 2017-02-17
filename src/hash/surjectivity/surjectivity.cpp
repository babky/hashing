#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <utility>

#include "boost/multiprecision/cpp_int.hpp"

#include "table.h"
#include "systems/cwlf_system.h"
#include "systems/multiply_shift_system.h"
#include "storages/chained_storage.h"
#include "utils/equality_comparer.h"
#include "iterators/set_iterator.h"
#include "experiments/traits.h"
#include "experiments/longest_chain.h"
#include "experiments/surjectivity.h"
#include "math/combinatorics.h"

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
using namespace Hash::Iterators;
using namespace Hash::Experiments;
using namespace Hash::Math;

typedef Table<size_t, Hash::Utils::EqualityComparer<size_t>, UniversalFunctionCWLF, Hash::Storages::ChainedStorage> TableCWLF;
typedef Table<size_t, Hash::Utils::EqualityComparer<size_t>, MultiplyShiftSystem, Hash::Storages::ChainedStorage> TableMultiplyShift;

void print_result(const ElementVector & v, const SurjectivityResult & surjectivityResult, const LongestChainResult & longestChainResult) {
	cout << v << " " << surjectivityResult.surjectiveCount << "/" << surjectivityResult.functionCount << " "
			<< longestChainResult.sum << "/" << longestChainResult.count << " "
			<< boost::multiprecision::cpp_rational(longestChainResult.sum, longestChainResult.count).convert_to<double>() <<  "\n";
}

template<class Table, bool intermediate>
void perform_experiment(size_t universeSize, size_t setSize, size_t tableSize, const ElementVector & fixedElements, const ElementVector & disabled, bool verbose) {
	ElementVector v = first(setSize, fixedElements, disabled);
	SurjectivityResult surjectivityResult(0, 0);
	LongestChainResult longestChainResult;

	bool first = true;
	SurjectivityResult sResult(0, 0);
	ElementVector sVector;
	LongestChainResult lResult;
	ElementVector lVector;

	// We fix 0, 1, ..., fixedCount. But we choose only from offset, offset + 1, ..., universeSize - 1
	// and the first elements are fixed.
	size_t fixedCount = fixedElements.size();
	unsigned long long count = binomial_coefficient(universeSize - fixedCount, setSize - fixedCount);
	unsigned long long current = 0;
	size_t percent = 0;

	for (bool shouldContinue = true; shouldContinue; shouldContinue = (setSize > fixedCount) && next(v, universeSize - 1, fixedElements, disabled)) {
		typedef typename Table::HashFunction::Generator Generator;
		typedef typename Table::HashFunction HashFunction;

		++current;
		if (verbose && percent != current * 100 / count) {
			percent = current * 100 / count;
			cout << percent << " %\n";
		}

		// Generator gs = GeneratorFactoryTraits<HashFunction>::create_generator(universeSize, tableSize);
		// surjectivityResult = surjective_number<HashFunction, Generator>(v, gs);

		Generator gl = GeneratorFactoryTraits<HashFunction>::create_generator(universeSize, setSize);
		longestChainResult = longest_chain<Table, HashFunction, Generator>(v, gl);

		if (intermediate) {
			print_result(v, surjectivityResult, longestChainResult);
		}

		if (first) {
			first = false;
			sVector = v;
			sResult = surjectivityResult;

			lVector = v;
			lResult = longestChainResult;
			if (verbose) {
				print_result(v, surjectivityResult, longestChainResult);
			}
		} else {
			/*if (surjectivityResult.surjectiveCount < sResult.surjectiveCount) {
				sVector = v;
				sResult = surjectivityResult;
			}*/

			if (longestChainResult.sum > lResult.sum) {
				lVector = v;
				lResult = longestChainResult;

				if (verbose) {
					print_result(v, surjectivityResult, longestChainResult);
				}
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

	Generator gl = GeneratorFactoryTraits<HashFunction>::create_generator(universeSize,
			Hash::Math::next_power(set.size()));
	LongestChainResult lResult = longest_chain<Table, HashFunction, Generator>(set, gl);

	cout << "Results\n";
	cout << "Surjectivity:  " << set << " " << sResult.surjectiveCount << "/" << sResult.functionCount << "\n";
	cout << "Longest chain: " << set << " " << lResult.sum << "/" << lResult.count << " "
			<< boost::multiprecision::cpp_rational(lResult.sum, lResult.count).convert_to<double>() << "\n";
}

/**
 * Performs the surjectivity and longest chain experiment for the interval_set.
 * The set is in the form {0, 1, ..., tableSize - 1}.
 */
template<class Table, bool intermediate>
void perform_partial_experiment_interval_set(size_t universeSize, size_t setSize, size_t tableSize) {
	ElementVector set;

	for (size_t i = 0; i < setSize; ++i) {
		set.push_back(i);
	}

	perform_partial_experiment<Table, intermediate>(universeSize, set, tableSize);
}


/**
 * Performs the surjectivity and longest chain experiment for the interval_set.
 * The set is in the form {0, 1, ..., log(tableSize)} \cup {tableSize, 2 * tableSize, ...}.
 */
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
	bool verbose = false;
	vector<size_t> elements;
	vector<size_t> disabled;
	string function = "cwlf";

	options_description optsDesc("Probability distribution computation.");
	optsDesc.add_options()\
		("help,h", "prints this help message")\
		("universe,u", value<size_t>(&universeSize)->default_value(universeSize), "Universe size")\
		("set,s", value<size_t>(&setSize)->default_value(setSize), "The size of the set.")\
		("table,t",	value<size_t>(&tableSize)->default_value(tableSize), "The size of the table.")\
		("elements,e", value<vector<size_t>>(&elements)->multitoken(), "The fixed elements.")\
		("disabled,d", value<vector<size_t>>(&disabled)->multitoken(), "The disabled elements.")\
		("function,f", value<string>(&function)->default_value(function), "Function type.")\
		("intermediate,i", value<bool>(&intermediate)->default_value(intermediate), "If the intermediate output should be provided.")\
		("verbose,v", value<bool>(&verbose)->default_value(verbose), "If percent of the computation should be printed.");

	variables_map vm;
	try {
		store(parse_command_line(argc, argv, optsDesc), vm);
		notify(vm);
	} catch (std::exception & e) {
		std::cerr << optsDesc;
		std::cerr << e.what() << std::endl;
		throw e;
	}

	if (vm.count("help")) {
		cout << optsDesc;
		return 0;
	}

	cout << "Function " << function << " with " << elements.size() << " fixed elements "
			<< (intermediate ? "with" : "without") << " immediate results." << endl;

	ElementVector fixedElements = elements;

	if (function == "cwlf") {
		if (intermediate) {
			perform_experiment<TableCWLF, true>(universeSize, setSize, tableSize, fixedElements, disabled, verbose);
		} else {
			perform_experiment<TableCWLF, false>(universeSize, setSize, tableSize, fixedElements, disabled, verbose);
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
			perform_experiment<TableMultiplyShift, true>(universeSize, setSize, tableSize, fixedElements, disabled, verbose);
		} else {
			perform_experiment<TableMultiplyShift, false>(universeSize, setSize, tableSize, fixedElements, disabled, verbose);
		}
	}

	return 0;
}

