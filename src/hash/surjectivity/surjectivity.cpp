#include <iostream>
#include <vector>
#include <numeric>
#include <utility>
#include "systems/cwlf_system.h"
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

bool next(ElementVector & v, size_t max) {
	size_t s = v.size() - 1;

	while (v[s] == max) {
		v[s] = v[s - 1] + 1;

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

ostream & operator <<(ostream & out, const ElementVector & v) {
	for (size_t i = 0; i < v.size(); ++i) {
		if (i > 0) {
			out << " ";
		}
		out << v[i];
	}
	return out;
}

template<class Function>
bool is_surjective(const ElementVector & v, Function f, bool * table) {
	for (size_t i = 0; i < f.getTableSize(); ++i) {
		table[i] = false;
	}

	for (size_t i = 0; i < v.size(); ++i) {
		// cout << "Hashing " << v[i] << " to " << f.hash(v[i]) << ".\n";

		table[f.hash(v[i])] = true;
	}

	return accumulate(table, table + f.getTableSize(), true, [](bool a, bool b) {return a & b;});
}

template<class Function, class FunctionGenerator>
pair<size_t, size_t> surjective_number(const ElementVector & v, FunctionGenerator g) {
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

	return make_pair(surjectiveCount, count);
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

typedef Table<size_t, Hash::Utils::EqualityComparer<size_t>, UniversalFunctionCWLF, Hash::Storages::ChainedStorage> SimpleTable;

template<class Function, class FunctionGenerator>
LongestChainResult longest_chain(const ElementVector & v, FunctionGenerator g) {
	size_t count = 0;
	size_t sum = 0;

	Function f = g.next();
	Hash::Utils::StorageStatistics stats;
	for (;; f = g.next()) {
		SimpleTable table(f);

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

int main(int argc, char ** argv) {
	using namespace boost::program_options;

	size_t prime = 17;
	size_t setSize = 8;
	size_t tableSize = 4;

	options_description optsDesc("Probability distribution computation.");
	optsDesc.add_options()("help", "prints this help message")("p", value<size_t>(&prime)->default_value(prime),
			"Universe")("s", value<size_t>(&setSize)->default_value(setSize), "The size of the set.")("t",
			value<size_t>(&tableSize)->default_value(tableSize), "The size of the table.");

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

	ElementVector v = first(setSize);
	pair<size_t, size_t> surjectivityResult;
	LongestChainResult longestChainResult;

	bool first = true;
	pair<size_t, size_t> sResult = make_pair(0, 0);
	ElementVector sVector;
	LongestChainResult lResult;
	ElementVector lVector;

	for (bool shouldContinue = true; shouldContinue; shouldContinue = next(v, prime - 1)) {
		SimpleTable::HashFunction::Generator gs(prime, tableSize);
		surjectivityResult = surjective_number<SimpleTable::HashFunction, SimpleTable::HashFunction::Generator>(v, gs);

		SimpleTable::HashFunction::Generator gl(prime, setSize);
		longestChainResult = longest_chain<SimpleTable::HashFunction, SimpleTable::HashFunction::Generator>(v, gl);

		cout << v << " " << surjectivityResult.first << "/" << surjectivityResult.second << " "
				<< longestChainResult.sum << "/" << longestChainResult.count << "\n";

		if (first) {
			first = false;
			sVector = v;
			sResult = surjectivityResult;

			lVector = v;
			lResult = longestChainResult;
		} else {
			if (surjectivityResult.first < sResult.first) {
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
	cout << "Surjectivity:  " << sVector << " " << sResult.first << "/" << sResult.second << "\n";
	cout << "Longest chain: " << lVector << " " << lResult.sum << "/" << lResult.count << "\n";

	return 0;
}
