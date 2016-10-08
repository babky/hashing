#include <iostream>
#include <vector>
#include <numeric>
#include <utility>
#include "systems/cwlf_system.h"
#ifdef BOOST_MSVC
	#pragma warning(disable: 4512)
#endif
#include <boost/program_options.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512)
#endif

using namespace std;
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

		if (s == 0) {
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

	return accumulate(table, table + f.getTableSize(), true, [](bool a, bool b) { return a & b; });
}

template<class Function, class FunctionGenerator>
pair<size_t, size_t> surjective_number(const ElementVector & v, FunctionGenerator g) {
	size_t count = 0;
	size_t surjectiveCount = 0;
	bool * table = 0;
	Function f = g.next();
	table = new bool[f.getTableSize()];
	for (; ; f = g.next()) {
		++count;
		if (is_surjective(v, f, table)) {
			++surjectiveCount;
		}

		if (!g.hasNext()) {
			break;
		}
	}
	delete [] table;
	table = 0;

	return make_pair(surjectiveCount, count);
}

int main(int argc, char ** argv) {
	using namespace boost::program_options;

	size_t prime = 17;
	size_t setSize = 8;
	size_t tableSize = 4;

	options_description optsDesc("Probability distribution computation.");
	optsDesc.add_options()
		("help", "prints this help message")
		("p", value<size_t>(&prime)->default_value(prime), "Universe")
		("s", value<size_t>(&setSize)->default_value(setSize), "The size of the set.")
		("t", value<size_t>(&tableSize)->default_value(tableSize), "The size of the table.");

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
	pair<size_t, size_t> result;

	pair<size_t, size_t> minResult = make_pair(0, 0);
	bool first = true;
	ElementVector minVector;

	for (bool shouldContinue = true; shouldContinue; shouldContinue = next(v, prime - 1)) {
		UniversalFunctionCWLF<size_t, void>::Generator g(prime, tableSize);
		result = surjective_number<UniversalFunctionCWLF<size_t, void>, UniversalFunctionCWLF<size_t, void>::Generator>(v, g);
		cout << v << " " << result.first << "/" << result.second << "\n";

		if (first) {
			first = false;
			minVector = v;
			minResult = result;
		} else if (result.first < minResult.first) {
			minVector = v;
			minResult = result;
		}
	}

	cout << minVector << " " << minResult.first << "/" << minResult.second << "\n";

	return 0;
}
