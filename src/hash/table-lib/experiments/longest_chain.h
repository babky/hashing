#ifndef LONGEST_CHAIN_H
#define LONGEST_CHAIN_H

#include <cstring>
#include "../storages/chained_storage.h"
#include "../utils/hash_math.h"
#include "../utils/storage_statistics.h"
#include "../iterators/set_iterator.h"

namespace Hash { namespace Experiments {

template<class Table, template <class> class GeneratorFactoryTraits, template <class> class ResultGatherer>
void compute_longest_chain_result(std::size_t universeSize, const Hash::Iterators::ElementVector & set, std::size_t tableSize, ResultGatherer<Table> & gatherer) {
	typedef typename Table::HashFunction::Generator Generator;
	typedef typename Table::HashFunction HashFunction;

	std::size_t setSize = set.size();
	Generator g = GeneratorFactoryTraits<HashFunction>::create_generator(universeSize, setSize);

	HashFunction f = g.next();
	for (;; f = g.next()) {
		Table table(f);
		for (std::size_t i = 0; i < set.size(); ++i) {
			table.insert(set[i]);
		}
		gatherer.gather(table);
		if (!g.hasNext()) {
			break;
		}
	}
}

struct AverageLongestChainResult {
	AverageLongestChainResult():
		AverageLongestChainResult(0, 0) {
	}

	AverageLongestChainResult(size_t aSum, size_t aCount):
		sum(aSum),
		count(aCount) {
	}

	std::size_t sum;
	std::size_t count;
};

template <class Table>
class AverageLongestChainResultGatherer {
public:
	void gather(const Table & table) {
		stats.clear();
		table.computeStatistics(stats);
		++result.count;
		result.sum += stats.getMaxChainLength();
	}

	AverageLongestChainResult getAverageLongestChainResult() const {
		return result;
	}

private:
	AverageLongestChainResult result;
	Hash::Utils::StorageStatistics stats; // optimization
};

template<class Table, template <class> class GeneratorFactoryTraits>
AverageLongestChainResult compute_longest_chain_average(std::size_t universeSize, const Hash::Iterators::ElementVector & set, std::size_t tableSize) {
	AverageLongestChainResultGatherer<Table> gatherer;
	compute_longest_chain_result<Table, GeneratorFactoryTraits, AverageLongestChainResultGatherer>(universeSize, set, tableSize, gatherer);
	return gatherer.getAverageLongestChainResult();
}

struct FindWorstSetResult {
	AverageLongestChainResult result;
	Hash::Iterators::ElementVector set;
};

template<class Table, template <class> class GeneratorFactoryTraits>
FindWorstSetResult find_worst_set(std::size_t universeSize, std::size_t setSize, std::size_t tableSize, const Hash::Iterators::ElementVector & fixedElements, const Hash::Iterators::ElementVector & disabled) {
	Hash::Iterators::ElementVector v = Hash::Iterators::first(setSize, fixedElements, disabled);
	AverageLongestChainResult longestChainResult;

	bool first = true;
	FindWorstSetResult lResult;

	// We fix 0, 1, ..., fixedCount. But we choose only from offset, offset + 1, ..., universeSize - 1
	// and the first elements are fixed.
	std::size_t fixedCount = fixedElements.size();

	for (bool shouldContinue = true; shouldContinue; shouldContinue = (setSize > fixedCount) && Hash::Iterators::next(v, universeSize - 1, fixedElements, disabled)) {
		longestChainResult = compute_longest_chain_average<Table, GeneratorFactoryTraits>(universeSize, v, tableSize);

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

	AverageLongestChainResult result;

};

template<class Table>
struct LongestChainHistogramGatherer {
public:
	LongestChainHistogramGatherer():
		sum(0),
		count(0) {
	}

	void gather(const Table & table) {
		stats.clear();
		table.computeStatistics(stats);
		sum += stats.getMaxChainLength();
		++count;

		LongestChainHistogram::Histogram::iterator it = histogram.find(stats.getMaxChainLength());
		if (it != histogram.end()) {
			++it->second;
		} else {
			histogram[stats.getMaxChainLength()] = 1;
		}
	}

	LongestChainHistogram getLongestChainResult() {
		LongestChainHistogram h;
		h.histogram = histogram;
		h.result = AverageLongestChainResult(sum, count);
		return h;
	}

private:
	LongestChainHistogram::Histogram histogram;
	size_t sum;
	size_t count;

	// Just for optimization reasons.
	Hash::Utils::StorageStatistics stats;
};

template<class Table, template <class> class GeneratorFactoryTraits>
LongestChainHistogram compute_longest_chain_histogram(std::size_t universeSize, const Hash::Iterators::ElementVector & set, std::size_t tableSize) {
	LongestChainHistogramGatherer<Table> gatherer;
	compute_longest_chain_result<Table, GeneratorFactoryTraits, LongestChainHistogramGatherer>(universeSize, set, tableSize, gatherer);
	return gatherer.getLongestChainResult();
}

template<
	typename T,
	typename Comparer,
	template <class, class> class Function,
	template <class, class, class> class Storage,
	class RehashPolicy = Policies::Rehash::LoadFactorBoundsRehashPolicy
>
class ExperimentTable : public Table<T, Comparer, Function, Storage, RehashPolicy> {
public:
	using typename Table<T, Comparer, Function, Storage, RehashPolicy>::HashStorage;
	using typename Table<T, Comparer, Function, Storage, RehashPolicy>::HashFunction;
	using Table<T, Comparer, Function, Storage, RehashPolicy>::storage;

	ExperimentTable(HashFunction f):
		Table<T, Comparer, Function, Storage, RehashPolicy>(f) {
	}

	const HashStorage & getStorage(void) const {
		return storage;
	}
};

template <typename T, typename EqualityComparer, typename HashType>
class ExperimentChainedStorage : public Hash::Storages::ChainedStorage<T, EqualityComparer, HashType> {
	using typename Hash::Storages::ChainedStorage<T, EqualityComparer, HashType>::ChainIterator;
	using typename Hash::Storages::ChainedStorage<T, EqualityComparer, HashType>::ChainedList;
public:
	typedef std::vector<T> Chain;
	using typename Hash::Storages::ChainedStorage<T, EqualityComparer, HashType>::Iterator;

	explicit ExperimentChainedStorage(size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE):
		Hash::Storages::ChainedStorage<T, EqualityComparer, HashType>(tableSize) {
	}

	explicit ExperimentChainedStorage(const EqualityComparer & comparer, size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE):
		Hash::Storages::ChainedStorage<T, EqualityComparer, HashType>(comparer, tableSize) {
	}

	ExperimentChainedStorage(const ExperimentChainedStorage & storage):
		Hash::Storages::ChainedStorage<T, EqualityComparer, HashType>(storage) {
	}

	Chain findLongestChain() const {
		ChainedList * max = &(this->storage[0]);
		for (size_t i = 0; i < this->storageInfo.getTableSize(); ++i) {
			if (max->getSize() < this->storage[i].getSize()) {
				max = &(this->storage[i]);
			}
		}

		Chain chain;
		chain.reserve(max->getSize());
		for (ChainIterator it = max->getBeginning(); it != max->getEnd(); ++it) {
			chain.push_back(*it);
		}
		return chain;
	}
};

template<template <class, class> class Function>
struct CompleteLongestChainInformationTraits {
	typedef Hash::Utils::EqualityComparer<size_t> Comparer;
	typedef ExperimentTable<size_t, Comparer, Function, ExperimentChainedStorage, Policies::Rehash::LoadFactorBoundsRehashPolicy> Table;
	typedef typename Table::HashStorage Storage;
	typedef typename ExperimentTable<size_t, Comparer, Function, ExperimentChainedStorage, Policies::Rehash::LoadFactorBoundsRehashPolicy>::HashFunction HashFunction;
};


template<class Function>
struct CompleteLongestChainInformation {
	typedef Function HashFunction;
	struct SingleChainInformation {
		SingleChainInformation(const Hash::Iterators::ElementVector & aChain, HashFunction aFunction):
			chain(aChain),
			function(aFunction) {
		}

		Hash::Iterators::ElementVector chain;
		HashFunction function;
	};
	typedef std::vector<SingleChainInformation> Chains;
	typedef std::map<std::size_t, Chains> ChainInformation;

	explicit CompleteLongestChainInformation(const ChainInformation & aInfo):
		info(aInfo) {
	}

	ChainInformation info;
};

template<class Table>
struct CompleteLongestChainInformationGatherer {
	typedef typename Table::HashFunction HashFunction;
	typedef typename CompleteLongestChainInformation<HashFunction>::ChainInformation ChainInformation;
	typedef typename CompleteLongestChainInformation<HashFunction>::SingleChainInformation SingleChainInformation;
	typedef typename CompleteLongestChainInformation<HashFunction>::Chains Chains;

	CompleteLongestChainInformationGatherer() {
	}

	void gather(const Table & table) {
		Hash::Iterators::ElementVector chain = table.getStorage().findLongestChain();
		std::size_t maxChainLength = chain.size();

		typename ChainInformation::iterator it = info.find(maxChainLength);
		if (it == info.end()) {
			it = info.insert({maxChainLength, Chains()}).first;
		}

		it->second.push_back(SingleChainInformation(chain, table.getFunction()));
	}

	CompleteLongestChainInformation<HashFunction> getCompleteLongestChainInformation(void) const {
		return CompleteLongestChainInformation<HashFunction>(info);
	}

private:
	ChainInformation info;
};
template<template <class, class> class Function, template <class> class GeneratorFactoryTraits>
CompleteLongestChainInformation<typename CompleteLongestChainInformationTraits<Function>::HashFunction> compute_longest_chain_complete_information(std::size_t universeSize, const Hash::Iterators::ElementVector & set, std::size_t tableSize) {
	typedef typename CompleteLongestChainInformationTraits<Function>::Comparer Comparer;
	typedef typename CompleteLongestChainInformationTraits<Function>::Table Table;
	typedef typename CompleteLongestChainInformationTraits<Function>::HashFunction HashFunction;

	CompleteLongestChainInformationGatherer<Table> gatherer;
	compute_longest_chain_result<Table, GeneratorFactoryTraits, CompleteLongestChainInformationGatherer>(universeSize, set, tableSize, gatherer);
	return gatherer.getCompleteLongestChainInformation();
}


} }

#endif
