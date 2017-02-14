#include <iostream>
#include <eo>
#include "table.h"
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
using namespace Hash::Iterators;
using namespace Hash::Utils;

typedef Table<size_t, Hash::Utils::EqualityComparer<size_t>, MultiplyShiftSystem, Hash::Storages::ChainedStorage> TableMultiplyShift;

struct Settings {
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

	bool optimizeMutationProbabilityMultipleElements;
	bool optimizeMutationProbabilityMultipleElementsPerElement;
};


class IndividualBadSet: public EO<size_t> {
	size_t universeMax;
	size_t setSize;
	ElementVector set;

public:
	IndividualBadSet() :
			universeMax(0), setSize(0), set(0) {
	}

	IndividualBadSet(size_t aUniverseMax, size_t aSetSize) :
			universeMax(aUniverseMax), setSize(aSetSize), set(aSetSize) {
	}

	void printOn(std::ostream & os) const {
		os << "Individual: [" << set << "], fitness: " << fitness() << ".";
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
	friend class BadSetMutation;
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
			hash_assert(*it >= 0 && *it <= settings.universeMax, "The value must be in the range 0 - universeMax.", __FILE__, __LINE__);
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
			LongestChainExperimentResult r = perform_longest_chain_experiment<TableMultiplyShift, GeneratorFactoryTraits>(settings.universeMax + 1, settings.setSize, settings.setSize, fixed, empty);
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

		LongestChainExperimentResult r = perform_longest_chain_experiment<TableMultiplyShift, GeneratorFactoryTraits>(settings.universeMax + 1, settings.setSize, settings.setSize, fixed, empty);
		set = r.set;
		std::sort(set.begin(), set.end());
		return change;
	}

};

template<typename Table>
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
		typedef typename Table::HashFunction Function;
		typedef typename Function::Generator Generator;
		Generator g(universeMax, tableSize);
		LongestChainResult r = longest_chain<Table, Function, Generator>(individual.getSet(), g);
		// cout << "Set " << individual.getSet() << " fitness " << (double)r.sum/r.count << endl;
		individual.fitness(r.sum);
	}
};

void optimize(const Settings & settings) {
	rng.reseed(settings.seed);

	const size_t universeMax = settings.universeMax;
	const size_t setSize = settings.setSize;

	Fitness<TableMultiplyShift> fitness(universeMax, setSize);
	eoPop<IndividualBadSet> pop;
	for (size_t i = 0; i < settings.populationSize; ++i) {
		IndividualBadSet v(universeMax, setSize);
		v.fillRandomly();
		fitness(v);
		pop.push_back(v);
	}

	eoDetTournamentSelect<IndividualBadSet> select(settings.tournamentSize);
	BadSetCrossOver xover;
	BadSetMutation mutation(settings);

	eoGenContinue<IndividualBadSet> continuator(settings.generationCount);

	eoSGA<IndividualBadSet> gga(select, xover, settings.crossoverProbability, mutation, settings.mutationProbability, fitness, continuator);
	gga(pop);

	pop.sort();
	cout << "The best solution found: " << pop[0] << endl;
	cout << "Final Population\n" << pop << endl;

	LongestChainHistogram histogram = compute_histogram<TableMultiplyShift, GeneratorFactoryTraits>(settings.universeMax, pop[0].getSet(), settings.setSize);
	cout << "L C\n";
	for (LongestChainHistogram::Histogram::iterator it = histogram.histogram.begin(); it != histogram.histogram.end(); ++it) {
		cout << it->first << " " << it->second << "\n";
	}
}

bool readSettingsFromArguments(Settings & settings, int argc, char ** argv) {
	using namespace boost::program_options;
	size_t universeSize = settings.universeMax + 1;

	options_description optsDesc("Probability distribution computation.");
	optsDesc.add_options()\
		("help,h", "prints this help message")\
		("universe,u", value<size_t>(&universeSize)->default_value(universeSize), "Universe size")\
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
		("optimize-mutation-probability-per-element", value<float>(&settings.optimizeMutationProbabilityPerElement)->default_value(settings.optimizeMutationProbabilityPerElement), "The probability of optimizing given element.");

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
	settings.universeMax = 63;
	settings.setSize = 8;
	settings.crossoverProbability = 0.4;
	settings.generationCount = 512;
	settings.mutationProbability = 1.0;
	settings.populationSize = 32;
	settings.tournamentSize = 3;
	settings.seed = 42;
	settings.shiftSingleElementMutationProbability = 0.1;
	settings.shiftSingleElementMutationProbabilityPerElement = 0.75;
	settings.shiftSingleElementMutationShift = 7;
	settings.optimizeMutationProbability = 0.1;
	settings.optimizeMutationProbabilityPerElement = 0.1;
	settings.optimizeMutationProbabilityMultipleElements = 0.1;
	settings.optimizeMutationProbabilityMultipleElementsPerElement = 0.05;

	bool shouldStop = readSettingsFromArguments(settings, argc, argv);
	if (shouldStop) {
		return 0;
	}

	optimize(settings);
	return 0;
}
