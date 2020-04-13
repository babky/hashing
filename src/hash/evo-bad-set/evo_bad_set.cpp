#include <iostream>
#include <eo>
#include "table.h"
#include "systems/bad_linear_system.h"
#include "systems/cwlf_system.h"
#include "systems/multiply_shift_system.h"
#include "storages/chained_storage.h"
#include "utils/equality_comparer.h"
#include "utils/hash_assert.h"
#include "utils/hash_math.h"
#include "iterators/set_iterator.h"
#include "experiments/traits.h"
#include "experiments/longest_chain.h"
#ifdef BOOST_MSVC
#pragma warning(disable: 4512)
#endif
#include <boost/program_options.hpp>
#ifdef BOOST_MSVC
#pragma warning(default: 4512)
#endif

using namespace std;
using namespace Hash;
using namespace Hash::Experiments;
using namespace Hash::Systems;
using namespace Hash::Storages;
using namespace Hash::Iterators;
using namespace Hash::Utils;

/**
 * Uses genetic algorithm for finding a bad set.
 */

typedef ChainedStorage<size_t, EqualityComparer<size_t>, size_t> Storage;

struct Settings {
	string system;

	size_t universeMax;
	size_t setSize;

	size_t seed;
	size_t populationSize;
	size_t tournamentSize;
	size_t generationCount;
	float crossoverProbability;

	float mutationProbability;
	float shiftSingleElementMutationProbability;
	float shiftSingleElementMutationProbabilityPerElement;
	size_t shiftSingleElementMutationShift;

	float optimizeMutationProbability;
	float optimizeMutationProbabilityPerElement;

	float optimizeMutationProbabilityMultipleElements;
	float optimizeMutationProbabilityMultipleElementsPerElement;
};

class IndividualBadSet: public EO<size_t> {
	size_t universeMax;
	size_t setSize;
	ElementVector set;

public:
	IndividualBadSet():
		universeMax(0), setSize(0), set(0) {
	}

	IndividualBadSet(size_t aUniverseMax, size_t aSetSize):
		universeMax(aUniverseMax), setSize(aSetSize), set(aSetSize) {
	}

	void printOn(std::ostream & os) const {
		size_t f = fitness();
		double lbin = static_cast<double>(f) / ((universeMax + 1) / 2);
		os << "Individual: [" << set << "], fitness: " << f << ", lbin: " << lbin << ".";
	}

	void fillRandomly() {
		size_t x;
		set.clear();
		for (size_t i = 0; i < setSize; ++i) {
			 for (x = rng.random(universeMax); contains(set, x); x = rng.random(universeMax)) {
			 }
			 set.push_back(x);
		}

		std::sort(set.begin(), set.end());
	}

	const ElementVector & getSet() const {
		return set;
	}

public:
	friend class BadSetCrossOver;
	template<class Table, template <class> class GeneratorFactoryTraits> friend class BadSetMutation;
};

class BadSetCrossOver : public eoQuadOp<IndividualBadSet> {
public:
	bool operator()(IndividualBadSet & i1, IndividualBadSet & i2) {
		ElementVector & set1 = i1.set;
		ElementVector & set2 = i2.set;
		bool change = false;

		size_t crossPosition = rng.random(set1.size());

		for (size_t i = 0; i < crossPosition && i < set1.size(); ++i) {
			if (contains(set2, set1[i])) {
				++crossPosition;
				continue;
			}

			if (contains(set1, set2[i])) {
				++crossPosition;
				continue;
			}

			change = true;
			swap(set1[i], set2[i]);
		}

		std::sort(set1.begin(), set1.end());
		std::sort(set2.begin(), set2.end());

		return change;
	}
};

template<class Table, template <class> class GeneratorFactoryTraits>
class BadSetMutation : public eoMonOp<IndividualBadSet> {
	Settings settings;

public:
	BadSetMutation(const Settings & aSettings):
		settings(aSettings) {
	}

	bool operator()(IndividualBadSet & individual) {
		bool mutationByShifting = false;
		if (rng.flip(settings.shiftSingleElementMutationProbability)) {
			mutationByShifting = mutateByShifting(individual);
		}

		bool mutationByOptimizingSingleElement = false;
		if (rng.flip(settings.optimizeMutationProbability)) {
			mutationByOptimizingSingleElement = mutateByOptimizingSingleElement(individual);
		}

		bool mutationByOptimizingMultipleElements = false;
		if (rng.flip(settings.optimizeMutationProbabilityMultipleElements)) {
			mutationByOptimizingMultipleElements = mutateByOptimizingMultipleElements(individual);
		}

		for (ElementVector::iterator it = individual.set.begin(); it != individual.set.end(); ++it) {
			hash_assert(
				*it >= 0 && *it <= settings.universeMax, "The value must be in the range 0 - universeMax.",
				__FILE__,
				__LINE__
			);
		}

		return mutationByShifting || mutationByOptimizingSingleElement;
	}

	bool mutateByShifting(IndividualBadSet & individual) {
		bool change = false;
		ElementVector & set = individual.set;
		size_t currentValue;
		size_t shift = settings.shiftSingleElementMutationShift;
		size_t halfShift = shift / 2;

		for (ElementVector::iterator it = set.begin(); it != set.end(); ++it) {
			if (!rng.flip(settings.shiftSingleElementMutationProbabilityPerElement)) {
				continue;
			}
			currentValue = *it;

			// Make it so that the mutation does not create elements outside the universe.
			*it = *it + rng.random(shift);
			if (halfShift > *it) {
				*it = 0;
			} else {
				*it -= halfShift;
			}

			if (*it > settings.universeMax) {
				*it = settings.universeMax;
			}

			// Make the mutation produce unique elements only.
			if (it != set.begin()) {
				if (*(it - 1) >= *it) {
					*it = *(it - 1) + 1;
				}
			}

			// Make the mutation produce unique elements only.
			if (it + 1 != set.end()) {
				if (*it >= *(it + 1)) {
					*it = *(it + 1) - 1;
				}
			}

			change = change || (*it != currentValue);
		}

		return change;
	}

	bool mutateByOptimizingSingleElement(IndividualBadSet & individual) {
		// Optimize by single element switching.
		ElementVector & set = individual.set;
		ElementVector empty;
		ElementVector fixed;

		bool change = false;

		size_t i = 0;
		for (ElementVector::iterator it = set.begin(); it != set.end(); ++it, ++i) {
			if (!rng.flip(settings.optimizeMutationProbabilityPerElement)) {
				continue;
			}
			// Find the best possible element for the set.
			fixed = set;
			fixed.erase(fixed.begin() + i);
			FindWorstSetResult r = find_worst_set<Table, GeneratorFactoryTraits>(
				settings.universeMax + 1,
				settings.setSize,
				settings.setSize,
				fixed,
				empty
			);
			*it = r.set[r.set.size() - 1];
			change = change || *it != r.set[r.set.size() - 1];
			*it = r.set[r.set.size() - 1];
		}

		std::sort(set.begin(), set.end());
		return change;
	}

	bool mutateByOptimizingMultipleElements(IndividualBadSet & individual) {
		// Optimize by single element switching.
		ElementVector & set = individual.set;
		ElementVector empty;
		ElementVector fixed = set;

		size_t i = 0, deleted = 0;
		bool change = false;
		for (ElementVector::iterator it = set.begin(); it != set.end(); ++it, ++i) {
			if (!rng.flip(settings.optimizeMutationProbabilityMultipleElementsPerElement)) {
				continue;
			}

			// Find the best possible element for the set.
			fixed.erase(fixed.begin() + i - deleted);
			++deleted;
			change = true;
		}

		if (!change) {
			return false;
		}

		FindWorstSetResult r = find_worst_set<Table, GeneratorFactoryTraits>(
			settings.universeMax + 1,
			settings.setSize,
			settings.setSize,
			fixed,
			empty
		);
		set = r.set;
		std::sort(set.begin(), set.end());
		return change;
	}

};

class BadSetContinuator : public eoGenContinue<IndividualBadSet>{
public:
	explicit BadSetContinuator(size_t generationCount):
		eoGenContinue<IndividualBadSet>(generationCount){
	}

	virtual bool operator() (const eoPop<IndividualBadSet> & pop) {
		bool retVal = eoGenContinue::operator()(pop);
		if (retVal) {
			cout << "Continuing to generation " << value() << " with population:\n";
			cout << pop << "\n" << endl;
		}
		return retVal;
	}
};

template<typename Table, template <class> class GeneratorFactoryTraits>
class Fitness: public eoEvalFunc<IndividualBadSet> {
	ElementVector badSet;
	size_t universeMax;
	size_t tableSize;

public:

	Fitness(size_t aUniverseMax, size_t aTableSize) :
			universeMax(aUniverseMax), tableSize(aTableSize) {
	}

	virtual ~Fitness(void) {
	}

	virtual void operator()(IndividualBadSet & individual) {
		AverageLongestChainResult r = compute_longest_chain_average<Table, GeneratorFactoryTraits>(
			universeMax,
			individual.getSet(),
			tableSize
		);
		individual.fitness(r.sum);
	}
};

template<template <class, class> class Function, template <class> class GeneratorFactoryTraits>
void optimize(const Settings & settings) {
	typedef Hash::Table<size_t, Hash::Utils::EqualityComparer<size_t>, Function, ChainedStorage> Table;

	rng.reseed(settings.seed);

	const size_t universeMax = settings.universeMax;
	const size_t setSize = settings.setSize;

	Fitness<Table, GeneratorFactoryTraits> fitness(universeMax, setSize);
	eoPop<IndividualBadSet> pop;
	for (size_t i = 0; i < settings.populationSize; ++i) {
		IndividualBadSet v(universeMax, setSize);
		v.fillRandomly();
		fitness(v);
		pop.push_back(v);
	}

	eoDetTournamentSelect<IndividualBadSet> select(settings.tournamentSize);
	BadSetCrossOver xover;
	BadSetMutation<Table, GeneratorFactoryTraits> mutation(settings);
	BadSetContinuator continuator(settings.generationCount);

	eoSGA<IndividualBadSet> gga(
		select,
		xover,
		settings.crossoverProbability,
		mutation,
		settings.mutationProbability,
		fitness,
		continuator
	);
	gga(pop);

	pop.sort();
	cout << "The best solution found: " << pop[0] << endl;
	cout << "Final Population\n" << pop << endl;

	LongestChainHistogram histogram = compute_longest_chain_histogram<Table, GeneratorFactoryTraits>(
		settings.universeMax,
		pop[0].getSet(),
		settings.setSize
	);
	cout << "L C\n";
	{
		const LongestChainHistogram::Histogram & h = histogram.histogram;
		for (LongestChainHistogram::Histogram::const_iterator it = h.begin(); it != h.end(); ++it) {
			cout << it->first << " " << it->second << "\n";
		}
	}

	typedef typename CompleteLongestChainInformationTraits<Function>::HashFunction HashFunction;
	typedef typename CompleteLongestChainInformation<HashFunction>::ChainInformation ChainInformation;
	typedef typename CompleteLongestChainInformation<HashFunction>::Chains Chains;
	CompleteLongestChainInformation<HashFunction> info =
		compute_longest_chain_complete_information<Function, GeneratorFactoryTraits>(
			settings.universeMax,
			pop[0].getSet(),
			settings.setSize
		);
	cout << "L C\n";
	for (typename ChainInformation::iterator it = info.info.begin(); it != info.info.end(); ++it) {
		cout << "---\n";
		cout << it->first << ": ";
		for (typename Chains::iterator cit = it->second.begin(); cit != it->second.end(); ++cit) {
			cout << "{" << std::to_string(cit->function) << ": " << "[" << cit->chain << "]} ";
		}
		cout << "\n";
	}
}

bool readSettingsFromArguments(Settings & settings, int argc, char ** argv) {
	using namespace boost::program_options;
	size_t universeSize = settings.universeMax + 1;

	options_description optsDesc("Probability distribution computation.");
	optsDesc.add_options()\
		("help,h", "prints this help message")\
		("function,f", value<string>(&settings.system)->default_value(settings.system), "System of functions (multiply-shift, bad-linear).")\
		("universe,u", value<size_t>(&universeSize)->default_value(universeSize), "Universe size.")\
		("set,s", value<size_t>(&settings.setSize)->default_value(settings.setSize), "The size of the set.")\
		("generation-count,g", value<size_t>(&settings.generationCount)->default_value(settings.generationCount), "The number of generations in the GA.")\
		("population-size,p", value<size_t>(&settings.populationSize)->default_value(settings.populationSize), "The size of the population in the GA.")\
		("tournament-size", value<size_t>(&settings.tournamentSize)->default_value(settings.tournamentSize), "The size of the tournament.")\
		("seed", value<size_t>(&settings.seed)->default_value(settings.seed), "Seed.")\
		("crossover-probability", value<float>(&settings.crossoverProbability)->default_value(settings.crossoverProbability), "CX probability.")\
		("mutation-probability", value<float>(&settings.mutationProbability)->default_value(settings.mutationProbability), "Mutation probability.")\
		("shift-single-element-mutation-probability", value<float>(&settings.shiftSingleElementMutationProbability)->default_value(settings.shiftSingleElementMutationProbability), "The probability of a mutation which shifts elements randomly a bit.")\
		("shift-single-element-mutation-probability-per-element", value<float>(&settings.shiftSingleElementMutationProbabilityPerElement)->default_value(settings.shiftSingleElementMutationProbabilityPerElement), "The probability of a change for a single element.")\
		("shift-single-element-mutation-shift", value<size_t>(&settings.shiftSingleElementMutationShift)->default_value(settings.shiftSingleElementMutationShift), "The size of the shift, uniform distribution is used.")\
		("optimize-mutation-probability", value<float>(&settings.optimizeMutationProbability)->default_value(settings.optimizeMutationProbability), "The probability of mutation which hides an element and tries to optimize the hidden value.")\
		("optimize-mutation-probability-per-element", value<float>(&settings.optimizeMutationProbabilityPerElement)->default_value(settings.optimizeMutationProbabilityPerElement), "The probability of optimizing given element.")\
		("optimize-mutation-probability-multiple-elements", value<float>(&settings.optimizeMutationProbabilityMultipleElements)->default_value(settings.optimizeMutationProbabilityMultipleElements), "The probability of mutation which hides multiple elements and tries to optimize the hidden values.")\
		("optimize-mutation-probability-multiple-elements-per-element", value<float>(&settings.optimizeMutationProbabilityMultipleElementsPerElement)->default_value(settings.optimizeMutationProbabilityMultipleElementsPerElement), "The probability of hiding a given element.");

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
		return true;
	}

	settings.universeMax = universeSize - 1;

	return false;
}

int main(int argc, char ** argv) {
	Settings settings;
	settings.system = "multiply-shift";
	settings.universeMax = 63;
	settings.setSize = 8;
	settings.crossoverProbability = 0.4;
	settings.generationCount = 512;
	settings.mutationProbability = 1/64;
	settings.populationSize = 32;
	settings.tournamentSize = 3;
	settings.seed = 42;
	settings.shiftSingleElementMutationProbability = 0.125;
	settings.shiftSingleElementMutationProbabilityPerElement = 0.75;
	settings.shiftSingleElementMutationShift = 7;
	settings.optimizeMutationProbability = 0.125;
	settings.optimizeMutationProbabilityPerElement = 0.1;
	settings.optimizeMutationProbabilityMultipleElements = 1 / 128;
	settings.optimizeMutationProbabilityMultipleElementsPerElement = 0.05 / 8;

	bool shouldStop = readSettingsFromArguments(settings, argc, argv);
	if (shouldStop) {
		return 0;
	}

	if (settings.system == "multiply-shift") {
		optimize<MultiplyShiftSystem, GeneratorFactoryTraits>(settings);
	} else if (settings.system == "bad-linear") {
		optimize<BadLinearSystem, GeneratorFactoryTraits>(settings);
	} else if (settings.system == "cwlf") {
		optimize<UniversalFunctionCWLF, RandomFunctionGeneratorFactoryTraits>(settings);
	} else {
		cerr << "Unknown family of functions " << settings.system << ".";
		return 1;
	}

	return 0;
}
