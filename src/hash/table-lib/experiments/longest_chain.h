#ifndef LONGEST_CHAIN_H
#define LONGEST_CHAIN_H

#include <cstring>
#include "../utils/hash_math.h"
#include "../utils/storage_statistics.h"
#include "../iterators/set_iterator.h"

namespace Hash { namespace Experiments {

struct LongestChainResult {

	LongestChainResult(void) :
			sum(0), count(0) {
	}

	LongestChainResult(std::size_t aSum, std::size_t aFunctionCount) :
			sum(aSum), count(aFunctionCount) {
	}

	std::size_t sum;
	std::size_t count;

};

struct LongestChainExperimentResult {

	LongestChainResult result;
	Hash::Iterators::ElementVector set;

};

template<class Table, class Function, class FunctionGenerator>
LongestChainResult longest_chain(const Hash::Iterators::ElementVector & v, FunctionGenerator g) {
	std::size_t count = 0;
	std::size_t sum = 0;

	Function f = g.next();
	Hash::Utils::StorageStatistics stats;
	for (;; f = g.next()) {
		Table table(f);
		stats.clear();

		for (std::size_t i = 0; i < v.size(); ++i) {
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

template<class Table, template <class> class GeneratorFactoryTraits>
LongestChainExperimentResult perform_longest_chain_experiment(std::size_t universeSize, std::size_t setSize, std::size_t tableSize, const Hash::Iterators::ElementVector & fixedElements, const Hash::Iterators::ElementVector & disabled) {
	Hash::Iterators::ElementVector v = Hash::Iterators::first(setSize, fixedElements, disabled);
	LongestChainResult longestChainResult;

	bool first = true;
	LongestChainExperimentResult lResult;

	// We fix 0, 1, ..., fixedCount. But we choose only from offset, offset + 1, ..., universeSize - 1
	// and the first elements are fixed.
	std::size_t fixedCount = fixedElements.size();

	for (bool shouldContinue = true; shouldContinue; shouldContinue = (setSize > fixedCount) && Hash::Iterators::next(v, universeSize - 1, fixedElements, disabled)) {
		typedef typename Table::HashFunction::Generator Generator;
		typedef typename Table::HashFunction HashFunction;

		Generator gl = GeneratorFactoryTraits<HashFunction>::create_generator(universeSize, setSize);
		longestChainResult = longest_chain<Table, HashFunction, Generator>(v, gl);

		if (first) {
			first = false;

			lResult.result = longestChainResult;
			lResult.set = v;
		} else {
			if (longestChainResult.sum > lResult.result.sum) {
				lResult.set = v;
				lResult.result = longestChainResult;
			}
		}
	}

	return lResult;
}

struct LongestChainHistogram {

	typedef std::map<size_t, size_t> Histogram;

	Histogram histogram;

	LongestChainResult result;

};

template<class Table, template <class> class GeneratorFactoryTraits>
LongestChainHistogram compute_histogram(std::size_t universeSize, const Hash::Iterators::ElementVector & set, std::size_t tableSize) {
	typedef typename Table::HashFunction::Generator Generator;
	typedef typename Table::HashFunction HashFunction;

	LongestChainHistogram histogram;
	std::size_t count = 0;
	std::size_t sum = 0;
	std::size_t setSize = set.size();
	Generator g = GeneratorFactoryTraits<HashFunction>::create_generator(universeSize, setSize);

	HashFunction f = g.next();
	Hash::Utils::StorageStatistics stats;
	for (;; f = g.next()) {
		Table table(f);
		stats.clear();

		for (std::size_t i = 0; i < set.size(); ++i) {
			table.insert(set[i]);
		}

		table.computeStatistics(stats);
		sum += stats.getMaxChainLength();
		++count;

		LongestChainHistogram::Histogram::iterator it = histogram.histogram.find(stats.getMaxChainLength());
		if (it != histogram.histogram.end()) {
			++it->second;
		} else {
			histogram.histogram[stats.getMaxChainLength()] = 1;
		}

		if (!g.hasNext()) {
			break;
		}
	}

	histogram.result = LongestChainResult(sum, count);
	return histogram;
}


} }

#endif
