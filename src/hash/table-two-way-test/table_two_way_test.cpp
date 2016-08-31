#include "storages/collision_count_storage.h"
#include "systems/two_way_system.h"
#include "systems/cwlf_system.h"
#include "systems/polynomial_system.h"
#include "systems/linear_map_system.h"
#include "systems/two_way_system_randomized.h"
#include "table.h"
#include "utils/constant_comparer.h"
#include "generators/generator.h"
#include "generators/random_generator.h"
#include "generators/linear_generator.h"
#include "generators/unit_input_generator.h"
#include "tests/test_impl.h"

#include <iostream>
#include <fstream>
#include <ostream>

#ifdef BOOST_MSVC
	#pragma warning(disable: 4512)
#endif
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/integer_traits.hpp>
#include <boost/integer.hpp>
#include <boost/tokenizer.hpp>
#include <boost/thread/thread.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512)
#endif

using namespace Hash;
using namespace Hash::Utils;
using namespace Hash::Generators;
using namespace Hash::Systems;
using namespace Hash::Tests;
using namespace Hash::Storages;

using namespace std;
using namespace boost;
using namespace boost::program_options;

template<typename T, class Storage>
class TwoWaySystemPolynomial32 : public Hash::Systems::TwoWaySystem<T, Storage, PolynomialSystem32> {
};

template<typename T, class Storage>
class TwoWaySystemRandomizedCWLF : public Hash::Systems::TwoWaySystemRandomized<T, Storage, TwoWaySystemCWLF, TwoWaySystemCWLF> {
};

template<typename T, class Storage>
class RandomizedUniversalFunctionCWLF : public Hash::Systems::TwoWaySystemRandomized<T, Storage, UniversalFunctionCWLF, UniversalFunctionCWLF> {
};

#define TestStorage CollisionCountStorage

template<typename T, typename Comparer, typename Hash>
class CollisionCountStorage16b : public Hash::Storages::template CollisionCountStorage<T, Comparer, Hash, boost::uint16_t> {
public:
	explicit CollisionCountStorage16b(const Comparer & comparer, size_t tableLength = StorageParams::INITIAL_STORAGE_SIZE):
		CollisionCountStorage<T, Comparer, Hash>(comparer, tableLength) {
	}

	explicit CollisionCountStorage16b(size_t tableLength = StorageParams::INITIAL_STORAGE_SIZE):
		CollisionCountStorage<T, Comparer, Hash>(tableLength) {
	}

	CollisionCountStorage16b(const CollisionCountStorage16b<T, Comparer, Hash> & aStorage):
		CollisionCountStorage<T, Comparer, Hash>(aStorage) {
	}

	void swap(CollisionCountStorage16b<T, Comparer, Hash> & b) {
		((CollisionCountStorage16b<T, Comparer, Hash> *) this)->swap((CollisionCountStorage16b<T, Comparer, Hash> &) b);
	}
};

template<typename ValueType>
Test * AssembleTest(string system, bool twoWay, string generator, size_t threads, size_t testLength, bool acceptSeed) {
	if (system == "linear-map") {
		if (twoWay) {
			if (generator == "random") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemLinearMap, TestStorage>, Hash::Generators::RandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "unit-input") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemLinearMap, TestStorage>, Hash::Generators::UnitInputGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "linear") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemLinearMap, TestStorage>, Hash::Generators::LinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		} else {
			if (generator == "random") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, UniversalFunctionLinearMap, TestStorage>, Hash::Generators::RandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "unit-input") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, UniversalFunctionLinearMap, TestStorage>, Hash::Generators::UnitInputGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "linear") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, UniversalFunctionLinearMap, TestStorage>, Hash::Generators::LinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		}
	} else if (system == "randomized-cwlf") {
		if (twoWay) {
			if (generator == "random") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemRandomizedCWLF, TestStorage>, Hash::Generators::RandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "unit-input") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemRandomizedCWLF, TestStorage>, Hash::Generators::UnitInputGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "linear") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemRandomizedCWLF, TestStorage>, Hash::Generators::LinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		} else {
			if (generator == "random") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, RandomizedUniversalFunctionCWLF, TestStorage>, Hash::Generators::RandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "unit-input") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, RandomizedUniversalFunctionCWLF, TestStorage>, Hash::Generators::UnitInputGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "linear") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, RandomizedUniversalFunctionCWLF, TestStorage>, Hash::Generators::LinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		}
	} else if (system == "cwlf") {
		if (twoWay) {
			if (generator == "random") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemCWLF, TestStorage>, Hash::Generators::RandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "unit-input") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemCWLF, TestStorage>, Hash::Generators::UnitInputGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "linear") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemCWLF, TestStorage>, Hash::Generators::LinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		} else {
			if (generator == "random") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, UniversalFunctionCWLF, TestStorage>, Hash::Generators::RandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "unit-input") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, UniversalFunctionCWLF, TestStorage>, Hash::Generators::UnitInputGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "linear") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, UniversalFunctionCWLF, TestStorage>, Hash::Generators::LinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		}
	} else if (system == "polynomial") {
		if (twoWay) {
			if (generator == "random") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemPolynomial, TestStorage>, Hash::Generators::RandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "unit-input") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemPolynomial, TestStorage>, Hash::Generators::UnitInputGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "linear") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemPolynomial, TestStorage>, Hash::Generators::LinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		} else {
			if (generator == "random") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, PolynomialSystem, TestStorage>, Hash::Generators::RandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "unit-input") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, PolynomialSystem, TestStorage>, Hash::Generators::UnitInputGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "linear") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, PolynomialSystem, TestStorage>, Hash::Generators::LinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		}
	} else if (system == "polynomial-32") {
		if (twoWay) {
			if (generator == "random") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemPolynomial32, TestStorage>, Hash::Generators::RandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "unit-input") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemPolynomial32, TestStorage>, Hash::Generators::UnitInputGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "linear") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemPolynomial32, TestStorage>, Hash::Generators::LinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		} else {
			if (generator == "random") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, PolynomialSystem32, TestStorage>, Hash::Generators::RandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "unit-input") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, PolynomialSystem32, TestStorage>, Hash::Generators::UnitInputGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "linear") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, PolynomialSystem32, TestStorage>, Hash::Generators::LinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		}
	} else if (system == "cwlf-exponential") {
		if (twoWay) {
			if (generator == "random") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemCWLFExponential, TestStorage>, Hash::Generators::RandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "unit-input") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemCWLFExponential, TestStorage>, Hash::Generators::UnitInputGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "linear") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemCWLFExponential, TestStorage>, Hash::Generators::LinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		} else {
			if (generator == "random") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemCWLFExponential, TestStorage>, Hash::Generators::RandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "unit-input") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemCWLFExponential, TestStorage>, Hash::Generators::UnitInputGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else if (generator == "linear") {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemCWLFExponential, TestStorage>, Hash::Generators::LinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		}
	}

	return 0;
}

int main(int argc, char ** argv) {
	// Parse the options.
#ifdef HASH_DEBUG
	const size_t DEFAULT_TEST_LENGTH = 32;
	const size_t DEFAULT_THREADS = 1;
#else
	const size_t DEFAULT_TEST_LENGTH = 1 << 20;
	const size_t DEFAULT_THREADS = 2;
#endif
	const size_t DEFAULT_REPEATS = 1 << 5;
	const string DEFAULT_SYSTEM = "linear-map";
	const bool DEFAULT_TWO_WAY = true;
	const bool DEFAULT_SEED = true;
	const string DEFAULT_GENERATOR = "unit-input";
	const string DEFAULT_APPENDIX = "";

	string outputFile;
	size_t threads;
	size_t testLength;
	size_t repeats;
	size_t bits;
	string system;
	string appendix;
	bool twoWay;
	bool seed;
	string generator;

	options_description optsDesc("Table Two Way Test allowed options.");
	optsDesc.add_options()
		("help", "prints this help message")
		("output-file", value<string>(&outputFile)->default_value(""), "file containing the test result")
		("threads", value<size_t>(&threads)->default_value(DEFAULT_THREADS), "number of filling threads")
		("test-length", value<size_t>(&testLength)->default_value(DEFAULT_TEST_LENGTH), "number of hashed elements")
		("repeats", value<size_t>(&repeats)->default_value(DEFAULT_REPEATS), "number of repeats of the test")
		("bits", value<size_t>(&bits), "base two logarithm of test-length")
		("system", value<string>(&system)->default_value(DEFAULT_SYSTEM), "system to be used [linear-map, polynomial, polynomial-32, cwlf, randomized-cwlf, cwlf-exponential]")
		("two-way", value<bool>(&twoWay)->default_value(DEFAULT_TWO_WAY), "should we use the two hashing")
		("generator", value<string>(&generator)->default_value(DEFAULT_GENERATOR), "should we use truly random set")
		("seed", value<bool>(&seed)->default_value(DEFAULT_SEED), "should we use random seed")
		("appendix", value<string>(&appendix)->default_value(DEFAULT_APPENDIX), "appendix added to the name of the file");

	variables_map vm;
	store(parse_command_line(argc, argv, optsDesc), vm);
	notify(vm);

	if (vm.count("help")) {
		cout << optsDesc;
		return 0;
	}

	if (vm.count("bits")) {
		testLength = 1 << bits;
	}

	if (outputFile.length() == 0) {
		stringstream ss;
		ss << "UHT-";
		ss << (twoWay ? "two-way" : "plain") << "-";
		ss << system << "-";
		ss << generator << "-";
		ss << (seed ? "random-seeded" : "not-seeded") << "-";
		ss << testLength << "e-";
		ss << repeats << "r-";
		ss << threads << "t-";
		if (appendix.length()) {
			ss << appendix << "-";
		}
		ss << "test.txt";

		outputFile = ss.str();
	}

	// Assemble the test.
	SmartPointer<Test> test(AssembleTest<boost::uint_fast64_t>(system, twoWay, generator, threads, testLength, seed));
	StorageStatistics stats;

	// Open the output.
	ofstream fout;
	if (outputFile != "--") {
		fout.open(outputFile.c_str());

		if (!fout.good()) {
			cerr << "Could not open output file'" << outputFile << "'.";
			return 2;
		}
	}

	// Use the output.
	ostream & out = fout.good() ? fout : cout;

	// Run it!
	for (size_t run = 1; run <= repeats; ++run) {
		out << "Running test " << run << ".\n";
		test->run();
		test->computeStatistics(stats);
		out << stats;
		out << endl;
	}

	return 0;
}
