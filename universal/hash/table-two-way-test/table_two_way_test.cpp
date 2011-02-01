#include "storages/collision_count_storage.h"
#include "systems/two_way_system.h"
#include "systems/cwlf_system.h"
#include "systems/polynomial_system.h"
#include "systems/linear_map_system.h"
#include "table.h"
#include "utils/constant_comparer.h"
#include "utils/static_random_generator.h"

#include <iostream>
#include <fstream>
#include <ostream>

#ifdef BOOST_MSVC
	#pragma warning(disable: 4512)
#endif
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/tokenizer.hpp>
#include <boost/thread/thread.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512)
#endif

using namespace Hash;
using namespace Hash::Utils;
using namespace Hash::Systems;
using namespace std;
using namespace boost;
using namespace boost::program_options;

template <typename T>
class TestLinearGenerator {
public:
	TestLinearGenerator(T aMin, T aMax, bool aAcceptSeed): 
	  min(aMin),
	  max(aMax),
	  i(0),
	  seed(1),
	  tableLength(0),
	  acceptSeed(aAcceptSeed) {
	}

	void setSeed(T aSeed) {
		seed = aSeed;
	}

	void setThreadNo(size_t) {
	}
	
	void setFrom(T from) {
		min = from;
	}

	void setPartLength(size_t) {
	}

	void setTableLength(size_t aTableLength) {
		tableLength = aTableLength;
	}

	void initialize(void) {
		if (seed % 2) {
			seed *= tableLength;
		}
		
		if (!acceptSeed) {
			seed = 1;
		}
	}
	
	T generate(void) {
		return (seed * i++) % (max - min) + min;
	}

private:
	T min;
	T max;
	T i;
	T seed;
	size_t tableLength;
	bool acceptSeed;
};

template <typename T>
class TestRandomGenerator {
public:
	static const size_t DEFAULT_SEED = 1;

	TestRandomGenerator(T aMin, T aMax, bool aAcceptSeed):
	  acceptSeed(aAcceptSeed),
	  generator(aMin, aMax) {
		generator.setSeed(DEFAULT_SEED);
	}

	void setSeed(int aSeed) {
		if (acceptSeed) {
			generator.setSeed(aSeed);
		}
	}

	void setThreadNo(size_t) {
	}
	
	void setFrom(T) {
	}

	void setPartLength(size_t) {
	}

	void setTableLength(size_t) {
	}
	
	void initialize(void) {
	}

	T generate(void) {
		return generator.generate();
	}

private:
	bool acceptSeed;
	RandomGenerator<T> generator;
};

class Test {
public:
	virtual ~Test(void) {
	}

	virtual void run(void) = 0;
	virtual void computeStatistics(Hash::Utils::StorageStatistics & stats) = 0;
};

template<typename ValueType, class TableType, class GeneratorType>
class TestImpl : public Test {
public:
	TestImpl(size_t aThreads, size_t aTestLength, size_t aTableLength, bool aAcceptSeed):
	  threads(aThreads), 
	  testLength(aTestLength),
	  tableLength(aTableLength),
	  table(aTableLength),
	  acceptSeed(aAcceptSeed) {
	}

	TableType & getTable(void) {
		return table;
	}

	const TableType & getTable(void) const {
		return const_cast<TestImpl *> (this)->getTable();
	}

	virtual void run(void) {
		table.clear();
		size_t partLength = testLength / threads;

		boost::thread ** testThreads = new boost::thread*[threads];
		size_t currentLength, from = 0;
		for (size_t t = 0; t < threads; ++t) {
			currentLength = t == 0 ? testLength - (threads - 1) * partLength : partLength;
			testThreads[t] = new boost::thread(
					TestPart(
						from,
						currentLength, 
						StaticRandomGenerator<size_t>::getGenerator().generate(),
						t,
						this
					)
				);

			from += currentLength;
		}

		for (size_t t = 0; t < threads; ++t) {
			testThreads[t]->join();
		}

		for (size_t t = 0; t < threads; ++t) {
			delete testThreads[t];
			testThreads[t] = 0;
		}

		delete [] testThreads;
	}

	virtual void computeStatistics(Hash::Utils::StorageStatistics & stats) {
		table.computeStatistics(stats);
	}

	bool getAcceptSeed(void) const {
		return acceptSeed;
	}
	
private:
	class TestPart {
	public:
		TestPart(size_t aFrom, size_t aPartLength, size_t seed, size_t aThreadNo, TestImpl<ValueType, TableType, GeneratorType> * aTest):
		  from(aFrom),
		  partLength(aPartLength),
		  generator(new GeneratorType(integer_traits<ValueType>::const_min, integer_traits<ValueType>::const_max, aTest->getAcceptSeed())),
		  threadNo(aThreadNo),
		  test(aTest) {
			generator->setPartLength(partLength);
			generator->setTableLength(test->getTable().getTableSize());
			generator->setSeed(seed);
			generator->setThreadNo(threadNo);
			generator->setFrom(from);
		}

		void operator()(void) {
			ValueType e;
			generator->initialize();
			ValueType universumMax = test->getTable().getFunction().getUniversumMax();

			for (size_t i = 0; i < partLength; ++i) {
				do {
					e = generator->generate();
				} while (e >= universumMax);

				test->getTable().insert(e);
			}
		}

	private:
		size_t from;
		size_t partLength;
		SmartPointer<GeneratorType> generator;
		size_t threadNo;
		TestImpl<ValueType, TableType, GeneratorType> * test;
	};

	TableType table;
	size_t threads;
	size_t testLength;
	size_t tableLength;
	bool acceptSeed;
};

template<typename T>
class PolynomialSystem32 : public Hash::Systems::PolynomialSystem<T> {
public:
	explicit PolynomialSystem32(size_t aStartLength = Hash::Systems::PolynomialSystem<T>::START_LENGTH, T aUniversumMax = Hash::Math::Prime<T>::GREATEST_PRIME, size_t aDegree = 32):
	  PolynomialSystem<T>(aStartLength, aUniversumMax, aDegree) {
	}
};

template<typename T>
class TwoWaySystemPolynomial32 : public Hash::Systems::TwoWaySystem<T, PolynomialSystem32> {
};

template<typename ValueType>
Test * AssembleTest(string system, bool twoWay, bool random, size_t threads, size_t testLength, bool acceptSeed) {
	if (system == "linear-map") {
		if (twoWay) {
			if (random) {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemLinearMap, Hash::Storages::CollisionCountStorage>, TestRandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemLinearMap, Hash::Storages::CollisionCountStorage>, TestLinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		} else {
			if (random) {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, UniversalFunctionLinearMap, Hash::Storages::CollisionCountStorage>, TestRandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, UniversalFunctionLinearMap, Hash::Storages::CollisionCountStorage>, TestLinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		}
	} else if (system == "cwlf") {
		if (twoWay) {
			if (random) {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemCWLF, Hash::Storages::CollisionCountStorage>, TestRandomGenerator<ValueType> >(threads, testLength, 2 * testLength, acceptSeed);
			} else {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemCWLF, Hash::Storages::CollisionCountStorage>, TestLinearGenerator<ValueType> >(threads, testLength, 2 * testLength, acceptSeed);
			}
		} else {
			if (random) {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, UniversalFunctionCWLF, Hash::Storages::CollisionCountStorage>, TestRandomGenerator<ValueType> >(threads, testLength, 2 * testLength, acceptSeed);
			} else {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, UniversalFunctionCWLF, Hash::Storages::CollisionCountStorage>, TestLinearGenerator<ValueType> >(threads, testLength, 2 * testLength, acceptSeed);
			}
		}
	} else if (system == "polynomial") {
		if (twoWay) {
			if (random) {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemPolynomial, Hash::Storages::CollisionCountStorage>, TestRandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemPolynomial, Hash::Storages::CollisionCountStorage>, TestLinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		} else {
			if (random) {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, PolynomialSystem, Hash::Storages::CollisionCountStorage>, TestRandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, PolynomialSystem, Hash::Storages::CollisionCountStorage>, TestLinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		}
	} else if (system == "polynomial-32") {
		if (twoWay) {
			if (random) {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemPolynomial32, Hash::Storages::CollisionCountStorage>, TestRandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemPolynomial32, Hash::Storages::CollisionCountStorage>, TestLinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		} else {
			if (random) {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, PolynomialSystem32, Hash::Storages::CollisionCountStorage>, TestRandomGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			} else {
				return new TestImpl<ValueType, Table<ValueType, ConstantComparer<ValueType>, PolynomialSystem32, Hash::Storages::CollisionCountStorage>, TestLinearGenerator<ValueType> >(threads, testLength, testLength, acceptSeed);
			}
		}
	} else {
		return 0;
	}
}

int main(int argc, char ** argv) {
	// Parse the options.
	const size_t DEFAULT_TEST_LENGTH = 1 << 16;
	const size_t DEFAULT_THREADS = 2;
	const size_t DEFAULT_REPEATS = 1 << 5;
	const string DEFAULT_SYSTEM = "linear-map";
	const bool DEFAULT_TWO_WAY = true;
	const bool DEFAULT_SEED = true;
	const bool DEFAULT_RANDOM = false;
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
	bool random;

	options_description optsDesc("Table Two Way Test allowed options.");
	optsDesc.add_options()
		("help", "prints this help message")
		("output-file", value<string>(&outputFile)->default_value(""), "file containing the test result")
		("threads", value<size_t>(&threads)->default_value(DEFAULT_THREADS), "number of filling threads")
		("test-length", value<size_t>(&testLength)->default_value(DEFAULT_TEST_LENGTH), "number of hashed elements")
		("repeats", value<size_t>(&repeats)->default_value(DEFAULT_REPEATS), "number of repeats of the test")
		("bits", value<size_t>(&bits), "base two logarithm of test-length")
		("system", value<string>(&system)->default_value(DEFAULT_SYSTEM), "system to be used [linear-map, polynomial, cwlf]")
		("two-way", value<bool>(&twoWay)->default_value(DEFAULT_TWO_WAY), "should we use the two hashing")
		("random", value<bool>(&random)->default_value(DEFAULT_RANDOM), "should we use truly random set")
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
		ss << (random ? "random" : "linear") << "-";
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
	SmartPointer<Test> test(AssembleTest<boost::uint_fast64_t>(system, twoWay, random, threads, testLength, seed));
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
