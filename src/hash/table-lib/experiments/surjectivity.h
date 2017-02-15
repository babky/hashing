#ifndef SURJECTIVITY_H
#define SURJECTIVITY_H

#include <cstring>
#include "../iterators/set_iterator.h"

namespace Hash { namespace Experiments {

struct SurjectivityResult {

	SurjectivityResult(std::size_t aSurjectiveCount, std::size_t aFunctionCount) :
			surjectiveCount(aSurjectiveCount), functionCount(aFunctionCount) {
	}

	std::size_t surjectiveCount;
	std::size_t functionCount;

};


template<class Function>
bool is_surjective(const Hash::Iterators::ElementVector & v, Function f, bool * table) {
	for (std::size_t i = 0; i < f.getTableSize(); ++i) {
		table[i] = false;
	}

	for (std::size_t i = 0; i < v.size(); ++i) {
		table[f.hash(v[i])] = true;
	}

	return accumulate(table, table + f.getTableSize(), true, [](bool a, bool b) {return a & b;});
}

template<class Function, class FunctionGenerator>
SurjectivityResult surjective_number(const Hash::Iterators::ElementVector & v, FunctionGenerator g) {
	std::size_t count = 0;
	std::size_t surjectiveCount = 0;
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

} }

#endif
