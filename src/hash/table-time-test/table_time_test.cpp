#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_set>
#include <string>
#include "utils/boost_include.h"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include "table.h"
#include "function.h"
#include "utils/equality_comparer.h"
#include "utils/equality_comparer.h"
#include "utils/storage_statistics.h"
#include "utils/generators/generators.h"
#include "universal_systems.h"
#include "storage.h"
#include "policies/guaranteed_rehash_policy.h"
#include "storages/chained_storage.h"
#include "storages/direct_chaining_storage.h"
#include "storages/probing_storage.h"
#include "systems/two_way_system_randomized.h"
#include "math/double_word.h"
#include "utils/time_vector.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::program_options;
using namespace Hash;
using namespace Hash::Utils;
using namespace Hash::Utils::Generators;
using namespace Hash::Systems;
using namespace Hash::Systems::Uniform;
using namespace Hash::Storages;
using namespace Hash::Policies::Rehash;

class Test {
public:
	virtual ~Test(void) {
	}

	virtual const string & getName(void) const = 0;
	virtual double getMaxLoadFactor(void) const = 0;
	virtual size_t getSize(void) const = 0;
	virtual size_t getRepeats(void) const = 0;
	virtual void runTest(size_t repeats) = 0;

	virtual const TimeVector & getInsertionTimes(void) const = 0;
	virtual const TimeVector & getSearchTimes(void) const = 0;
};

template<typename T, template<class> class Generator, class Table>
class TestImplBase : public Test {
public:
	TestImplBase(size_t aBits, string aTableName, string aGeneratorName, double aMaxLoadFactor, size_t aSize):
		bits(aBits),
		tableName(aTableName),
		generatorName(aGeneratorName),
		maxLoadFactor(aMaxLoadFactor),
		size(aSize) {
	}

	virtual double getMaxLoadFactor(void) const {
		return maxLoadFactor;
	}

	virtual size_t getSize(void) const {
		return size;
	}

	virtual const string & getName(void) const {
		return tableName;
	}

	virtual const TimeVector & getInsertionTimes(void) const {
		return insertionTimes;
	}

	virtual const TimeVector & getSearchTimes(void) const {
		return searchTimes;
	}

	virtual size_t getRepeats(void) const {
		return searchTimes.size();
	}

protected:
	size_t bits;
	string tableName;
	string generatorName;
	double maxLoadFactor;
	size_t size;

	Generator<T> g;

	TimeVector searchTimes;
	TimeVector insertionTimes;
};

template<typename T, template<class> class Generator, class Table>
class TestImpl : public TestImplBase<T, Generator, Table> {
public:
	using TestImplBase<T, Generator, Table>::g;
	using TestImplBase<T, Generator, Table>::maxLoadFactor;
	using TestImplBase<T, Generator, Table>::size;
	using TestImplBase<T, Generator, Table>::searchTimes;
	using TestImplBase<T, Generator, Table>::insertionTimes;

	TestImpl(size_t aBits, string aTableName, string aGeneratorName, double aMaxLoadFactor, size_t aSize):
		TestImplBase<T, Generator, Table>(aBits, aTableName, aGeneratorName, aMaxLoadFactor, aSize) {
	}

	virtual ~TestImpl(void) {
	}

	virtual void runTest(size_t repeats) {
		while (repeats != 0) {
			ptime start, finish;
			applyMaxLoadFactor(maxLoadFactor);
			clear();

			g.reset();
			start = microsec_clock::local_time();
			for (T i = 0; i < size; ++i) {
				t.insert(g.next());
			}
			finish = microsec_clock::local_time();
			insertionTimes.add(finish - start);

			g.reset();
			start = microsec_clock::local_time();
			for (T i = 0; i < size; ++i) {
#ifdef HASH_DEBUG
				T value = g.next();
				if (!t.contains(value)) {
					cerr << "Should contain " << value << "." << endl;
				}
#else
				t.contains(g.next());
#endif
			}

#ifdef HASH_DEBUG
			if (t.size() != size) {
				cerr << "Table should contain all the elements." << endl;
			}
#endif
			finish = microsec_clock::local_time();
			clear();
			searchTimes.add(finish - start);

			--repeats;
		}
	}

protected:
	void clear(void) {
		t.clear();
	}

	void applyMaxLoadFactor(double alpha) {
		t.setRehashPolicy(LoadFactorBoundsRehashPolicy(LoadFactorBoundsRehashPolicy::DEFAULT_MIN_LOAD_FACTOR, alpha));
	}

	Table t;
};

template<typename T, template<class> class Generator, class Structure>
class StlTestImpl : public TestImplBase<T, Generator, Structure> {
public:
	using TestImplBase<T, Generator, Structure>::g;
	using TestImplBase<T, Generator, Structure>::maxLoadFactor;
	using TestImplBase<T, Generator, Structure>::size;
	using TestImplBase<T, Generator, Structure>::searchTimes;
	using TestImplBase<T, Generator, Structure>::insertionTimes;

	StlTestImpl(size_t aBits, string aTableName, string aGeneratorName, double aMaxLoadFactor, size_t aSize):
		TestImplBase<T, Generator, Structure>(aBits, aTableName, aGeneratorName, aMaxLoadFactor, aSize) {
	}

	virtual ~StlTestImpl(void) {
	}

	virtual void runTest(size_t repeats) {
		while (repeats != 0) {
			ptime start, finish;
			clear();

			g.reset();
			start = microsec_clock::local_time();
			for (T i = 0; i < size; ++i) {
				t.insert(g.next());
			}
			finish = microsec_clock::local_time();
			insertionTimes.add(finish - start);

			g.reset();
			start = microsec_clock::local_time();
			for (T i = 0; i < size; ++i) {
#ifdef HASH_DEBUG
				T value = g.next();
				if (t.find(value) != t.end()) {
					cerr << "Should contain " << value << "." << endl;
				}
#else
				t.find(g.next());
#endif
			}

#ifdef HASH_DEBUG
			if (t.size() != size) {
				cerr << "Table should contain all the elements." << endl;
			}
#endif
			finish = microsec_clock::local_time();
			clear();
			searchTimes.add(finish - start);

			--repeats;
		}
	}

protected:
	void clear(void) {
		t.clear();
	}

	Structure t;
};

template<typename T, template<class> class Generator, template <class, class, class> class Storage, template <class, class> class Function>
Test * instantiateTest(size_t bits, double maxLoadFactor, size_t size, string tableName, string generatorName) {
	return new TestImpl<T, Generator, Table<T, EqualityComparer<T>, Function, Storage> >(bits, tableName, generatorName, maxLoadFactor, size);
}

template<typename T, template<class> class Generator, template <class, class, class> class Storage>
Test * decideFunction(size_t bits, double maxLoadFactor, size_t size, string tableName, string generatorName, string functionName) {
	if (functionName == "Id") {
		return instantiateTest<T, Generator, Storage, IdentityFunction>(bits, maxLoadFactor, size, tableName, generatorName);
	} else if (functionName == "MSVC") {
		return instantiateTest<T, Generator, Storage, Tr1Function>(bits, maxLoadFactor, size, tableName, generatorName);
	} else if (functionName == "Tabulation") {
		return instantiateTest<T, Generator, Storage, TabulationFunction>(bits, maxLoadFactor, size, tableName, generatorName);
	} else if (functionName == "BitString") {
		return instantiateTest<T, Generator, Storage, BitStringFunction>(bits, maxLoadFactor, size, tableName, generatorName);
	} else if (functionName == "CWLF") {
		return instantiateTest<T, Generator, Storage, UniversalFunctionCWLF>(bits, maxLoadFactor, size, tableName, generatorName);
	} else if (functionName == "Pol2") {
		return instantiateTest<T, Generator, Storage, PolynomialSystem2>(bits, maxLoadFactor, size, tableName, generatorName);
	} else if (functionName == "Pol4") {
		return instantiateTest<T, Generator, Storage, PolynomialSystem4>(bits, maxLoadFactor, size, tableName, generatorName);
	} else if (functionName == "Pol5") {
		return instantiateTest<T, Generator, Storage, PolynomialSystem5>(bits, maxLoadFactor, size, tableName, generatorName);
	} else if (functionName == "Pol32") {
		return instantiateTest<T, Generator, Storage, PolynomialSystem32>(bits, maxLoadFactor, size, tableName, generatorName);
	} else if (functionName == "DW4") {
		return instantiateTest<T, Generator, Storage, DietzfelbingerWoelfelPol4>(bits, maxLoadFactor, size, tableName, generatorName);
	} else if (functionName == "LinearMap") {
		return instantiateTest<T, Generator, Storage, UniversalFunctionLinearMap>(bits, maxLoadFactor, size, tableName, generatorName);
	} else {
		return 0;
	}
}

template<typename T, template<class> class Generator>
Test * decideStorage(size_t bits, double maxLoadFactor, size_t size, string tableName, string generatorName, string storageName, string functionName) {
	if (storageName == "Chaining") {
		return decideFunction<T, Generator, ChainedStorage>(bits, maxLoadFactor, size, tableName, generatorName, functionName);
	} else if (storageName == "DirectChaining") {
		return decideFunction<T, Generator, ChainedStorage>(bits, maxLoadFactor, size, tableName, generatorName, functionName);
	} else if (storageName == "LinearProbing") {
		return decideFunction<T, Generator, ChainedStorage>(bits, maxLoadFactor, size, tableName, generatorName, functionName);
	} else if (storageName == "RBT") {
		return new StlTestImpl<T, Generator, set<T> >(bits, tableName, generatorName, maxLoadFactor, size);
	} else if (storageName == "US") {
#ifdef BOOST_MSVC
		typedef std::tr1::unordered_set<T> UnorderedSet;
#else
		typedef unordered_set<T> UnorderedSet;
#endif
		return new StlTestImpl<T, Generator, UnorderedSet>(bits, tableName, generatorName, maxLoadFactor, size);
	} else {
		return 0;
	}
}

template<typename T, template<class> class Generator>
Test * decideTable(size_t bits, double maxLoadFactor, size_t size, string tableName, string generatorName) {
	vector<string> tableDesc;
	split(tableDesc, tableName, is_any_of(":"));

	const size_t STORAGE = 0;
	const size_t FUNCTION = 1;

	if (tableDesc.size() == 1) {
		return decideStorage<T, Generator>(bits, maxLoadFactor, size, tableName, generatorName, tableDesc[STORAGE], "");
	} else {
		return decideStorage<T, Generator>(bits, maxLoadFactor, size, tableName, generatorName, tableDesc[STORAGE], tableDesc[FUNCTION]);
	}
}

template<typename T>
Test * decideGenerator(size_t bits, double maxLoadFactor, size_t size, string tableName, string generatorName) {
	if (generatorName == "Linear") {
		return decideTable<T, LinearGenerator>(bits, maxLoadFactor, size, tableName, generatorName);
	} else if (generatorName == "Quadratic") {
		return decideTable<T, QuadraticGenerator>(bits, maxLoadFactor, size, tableName, generatorName);
	} else if (generatorName == "Shift") {
		return decideTable<T, ShiftGenerator>(bits, maxLoadFactor, size, tableName, generatorName);
	} else {
		return 0;
	}
}

Test * decideBits(size_t bits, double maxLoadFactor, size_t size, string tableName, string generatorName) {
	typedef boost::uint32_t T32;
	typedef boost::uint64_t T64;
	if (bits == 32) {
		return decideGenerator<T32>(bits, maxLoadFactor, size, tableName, generatorName);
	} else if (bits == 64) {
		return decideGenerator<T64>(bits, maxLoadFactor, size, tableName, generatorName);
	} else {
		return 0;
	}
}

Test * createTest(size_t bits, double maxLoadFactor, size_t size, string tableName, string generatorName) {
	return decideBits(bits, maxLoadFactor, size, tableName, generatorName);
}

int main(int argc, char ** argv) {
	size_t bits;
	size_t size;
	size_t sizeStep;
	size_t sizeSteps;
	double loadFactor;
	double loadFactorStep;
	double loadFactorSteps;
	size_t repeats;
	string generator;
	string outputFile;
	vector<string> tableList;

	options_description opts("Hash tables time test options.");
	opts.add_options()
		("bits", value<size_t>(&bits), "the number of bits used 32 or 64")
		("size", value<size_t>(&size), "the logarithm of size of the table")
		("sizeStep", value<size_t>(&sizeStep)->default_value(1), "the multiplicative factor of size")
		("sizeSteps", value<size_t>(&sizeSteps), "the number of steps for which the size is changed accordingly")
		("repeats", value<size_t>(&repeats)->default_value(32), "the number of repetitions")
		("loadFactor", value<double>(&loadFactor), "the initial max load factor")
		("loadFactorStep", value<double>(&loadFactorStep)->default_value(0.125), "the step for load factor")
		("loadFactorSteps", value<double>(&loadFactorSteps), "the number of steps for the load factor")
		("generator", value<string>(&generator), "the values generator")
		("output", value<string>(&outputFile), "output file")
		("tableList", value<vector<string> >(&tableList), "tested tables")
		("help", "prints this help.");

	positional_options_description popts;
	popts.add("tableList", -1);

	variables_map vm;
	store(command_line_parser(argc, argv).options(opts).positional(popts).run(), vm);
	notify(vm);

	if (vm.count("help")) {
		cout << opts;
		return 0;
	}

	typedef vector<double> LoadFactorVector;
	typedef vector<size_t> SizeVector;
	typedef vector<Test *> TestVector;

	LoadFactorVector loadFactors;
	SizeVector sizes;
	TestVector tests;

	for (size_t i = 0; i < loadFactorSteps; ++i) {
		loadFactors.push_back(i * loadFactorStep + loadFactor);
	}

	for (size_t i = 0; i < sizeSteps; ++i) {
		sizes.push_back(1 << (i * sizeStep + size));
	}

	Test * t;
	bool stop = false;
	for (vector<string>::const_iterator bT = tableList.begin(), eT = tableList.end(); bT != eT; ++bT) {
		t = createTest(bits, 0, 0, *bT, generator);
		if (t == 0) {
			cerr << "Could not create test " << *bT << ".\n";
			stop = true;
		}
	}
	if (stop) {
		return 1;
	}

	for (SizeVector::iterator bS = sizes.begin(), eS = sizes.end(); bS != eS; ++bS) {
		for (LoadFactorVector::iterator bLF = loadFactors.begin(), eLF = loadFactors.end(); bLF != eLF; ++bLF) {
			for (vector<string>::const_iterator bT = tableList.begin(), eT = tableList.end(); bT != eT; ++bT) {
				t = createTest(bits, *bLF, *bS, *bT, generator);
				cout << "Testing " << t->getName() << " [size=" << *bS << ", alpha=" << *bLF << "]\n";
				t->runTest(repeats);
				cout << "\tinsertion=" << t->getInsertionTimes().getAverageTime() << "\n";
				cout << "\t   search=" << t->getSearchTimes().getAverageTime() << "\n" << endl;
				tests.push_back(t);
			}
		}
	}

	ofstream fout;
	if (outputFile != "") {
		fout.open(outputFile.c_str());
	}

	ostream & out = outputFile != "" ? fout : cout;
	for (TestVector::const_iterator b = tests.begin(), e = tests.end(); b != e; ++b) {
		out << "TEST: name = " << setw(20) << (*b)->getName()
			<< ", size = " << setw(10) << (*b)->getSize()
			<< ", repeats = " << setw(2) << (*b)->getRepeats()
			<< ", alpha = " << setw(5) << (*b)->getMaxLoadFactor()
			<< "\n";
		out << "Insertion: " << setw(7) << (*b)->getSearchTimes().getAverageTime().total_milliseconds() << " [(+/-) " << setw(10) << fixed << setprecision(3) << (*b)->getSearchTimes().getMillisVariance() << "] ms\n";
		out << "   Search: " << setw(7) << (*b)->getInsertionTimes().getAverageTime().total_milliseconds() << " [(+/-) " << setw(10) << fixed << setprecision(3) << (*b)->getInsertionTimes().getMillisVariance() << "] ms\n";
		out << "\n";
	}

	out << "\n\n---PRINTING WHOLE DATA SET---\n";
	for (TestVector::const_iterator b = tests.begin(), e = tests.end(); b != e; ++b) {
		out << setw(20) << (*b)->getName()
			<< ", size = " << setw(10) << (*b)->getSize()
			<< ", repeats = " << setw(2) << (*b)->getRepeats()
			<< ", alpha = " << setw(5) << (*b)->getMaxLoadFactor()
			<< "\n";
		out << "Insertion: [" << (*b)->getSearchTimes() << "]\n";
		out << "   Search: [" << (*b)->getInsertionTimes() << "]\n";
		out << "\n";
	}

	if (outputFile != "") {
		fout.close();
	}

	return 0;
}

