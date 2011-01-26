#include "storages/collision_count_storage.h"
#include "systems/two_way_system.h"
#include "systems/cwlf_system.h"
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
class TwoWaySystemCWLF : public Hash::Systems::TwoWaySystem<T, Hash::UniversalFunctionCWLF> {
};

template <typename T>
class TwoWaySystemLinearMap : public Hash::Systems::TwoWaySystem<T, Hash::UniversalFunctionLinearMap> {
};

template<typename ValueType, class TableType>
class Test {
public:
	Test(size_t aThreads, size_t aTestLength):
	  threads(aThreads), 
	  testLength(aTestLength),
	  table(aTestLength) {
	}

	TableType & getTable(void) {
		return table;
	}

	const TableType & getTable(void) const {
		return const_cast<Test *> (this)->getTable();
	}

	void run(void) {
		table.clear();
		size_t partLength = testLength / threads;

		boost::thread ** testThreads = new boost::thread*[threads];
		for (size_t t = 0; t < threads; ++t) {
			testThreads[t] = new boost::thread(
					TestPart(
						t == 0 ? testLength - (threads - 1) * partLength : partLength, 
						&table,
						StaticRandomGenerator<size_t>::getGenerator().generate()
					)
				);
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
	
private:
	class TestPart {
	public:
		typedef RandomGenerator<ValueType> GeneratorType;
		
		TestPart(size_t aPartLength, TableType * aTable, size_t seed):
		  partLength(aPartLength),
		  table(aTable),
		  generator(new GeneratorType(integer_traits<ValueType>::const_min, integer_traits<ValueType>::const_max)) {
			generator->setSeed(seed);
		}

		void operator()(void) {
			ValueType e;
			for (size_t i = 0; i < partLength; ++i) {
				e = generator->generate();
				table->insert(e);
			}
		}

	private:
		size_t partLength;
		TableType * table;
		SmartPointer<GeneratorType> generator;
	};

	TableType table;
	size_t threads;
	size_t testLength;
};

int main(int argc, char ** argv) {
	const size_t DEFAULT_TEST_LENGTH = 1 << 10;
	const size_t DEFAULT_THREADS = 1;
	const size_t DEFAULT_REPEATS = 1 << 5;
	
	typedef boost::uint_fast64_t ValueType;
	typedef Table<ValueType, ConstantComparer<ValueType>, TwoWaySystemLinearMap, Hash::Storages::CollisionCountStorage> TableType;

	string outputFile;
	size_t threads;
	size_t testLength;
	size_t repeats;

	options_description optsDesc("Table Two Way Test allowed options.");
	optsDesc.add_options()
		("help", "prints this help message")
		("output-file", value<string>(&outputFile)->default_value(""))
		("threads", value<size_t>(&threads)->default_value(DEFAULT_THREADS))
		("test-length", value<size_t>(&testLength)->default_value(DEFAULT_TEST_LENGTH))
		("repeats", value<size_t>(&repeats)->default_value(DEFAULT_REPEATS));

	variables_map vm;
	store(parse_command_line(argc, argv, optsDesc), vm);
	notify(vm);

	if (vm.count("help")) {
		cout << optsDesc;
		return 0;
	}

	ofstream fout(outputFile.c_str());
	ostream & out = fout.good() ? fout : cout;

	Test<ValueType, TableType> test(threads, testLength);
	StorageStatistics stats;

	for (size_t run = 1; run <= repeats; ++run) {
		out << "Running test " << run << ".\n";
		test.run();
		test.getTable().computeStatistics(stats);
		out << stats;
		out << endl;
	}

	return 0;
}
