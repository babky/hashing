#include <iostream>
#include <vector>
#include <string>
#include <boost/config.hpp>
#include <boost/program_options.hpp>
#ifdef BOOST_MSVC
	#pragma warning(disable: 4512 4127 4100)
#endif
#include <boost/date_time.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512 4127 4100)
#endif
#include "systems/uniform/dietzfelbinger_woelfel.h"
#include "systems/bit_string_system.h"
#include "systems/polynomial_system.h"
#include "systems/linear_map_system.h"
#include "systems/cwlf_system.h"
#include "systems/tabulation_system.h"
#include "systems/polynomial_system.h"
#include "systems/tr1_function.h"
#include "systems/identity_function.h"

using namespace std;
using namespace boost::posix_time;

namespace Hash { namespace Systems {
	
	template<typename T, class Storage>
	class PolynomialSystem4 : public Hash::Systems::PolynomialSystem<T, Storage> {
	public:
		explicit PolynomialSystem4(size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE,
			T aUniversumMax = Hash::Math::Prime<T>::GREATEST_PRIME, size_t aDegree = 4):
		  PolynomialSystem<T, Storage>(tableSize, aUniversumMax, aDegree) {
		}
	};

} }

struct Test {
	Test(string n):
	  name(n) {
	}

	virtual void run(void) = 0;

protected:
	string name;
	time_duration time;
};

template<class F, typename T>
struct FunctionTest : public Test {

	FunctionTest(size_t aLength, string name):
	  Test(name),
	  length(aLength) {
	}

	virtual void run() {
		function.setTableSize(length);
		function.reset();

		ptime start = microsec_clock::local_time();
		size_t hash = 0xdead;
		for (size_t i = 0; i < length; ++i) {
			hash ^= function.hash(i);
		}
		ptime finish = microsec_clock::local_time();
		time = finish - start;

		if (hash == 0) {
			cout << "Almost impossible :)!\n";
		}
		
		cout << name << ": " << time.total_milliseconds() << " ms" << endl;
	}

private:
	size_t length;
	F function;
};

template<typename T>
void runTest(size_t length) {
	typedef size_t TestStorage;
	typedef vector<Test *> FunctionVector;
	using namespace Hash::Systems;

	FunctionVector functions;
	
	functions.push_back(new FunctionTest<IdentityFunction<T, TestStorage>, T>(length, "ID"));
	functions.push_back(new FunctionTest<Tr1Function<T, TestStorage>, T>(length, "TR1"));
	functions.push_back(new FunctionTest<UniversalFunctionCWLF<T, TestStorage>, T>(length, "CWLF"));
	functions.push_back(new FunctionTest<BitStringFunction<T, TestStorage>, T>(length, "BitString"));
	functions.push_back(new FunctionTest<TabulationFunction<T, TestStorage>, T>(length, "Tabulation"));
	functions.push_back(new FunctionTest<UniversalFunctionLinearMap<T, TestStorage>, T>(length, "LinearMap"));
	functions.push_back(new FunctionTest<PolynomialSystem<T, TestStorage>, T>(length, "Polynomial - deg 2"));
	functions.push_back(new FunctionTest<PolynomialSystem4<T, TestStorage>, T>(length, "Polynomial - deg 4"));
	functions.push_back(new FunctionTest<Uniform::DietzfelbingerWoelfel<T, TestStorage, Hash::Systems::PolynomialSystem4>, T>(length, "DW - deg 4"));

	for (FunctionVector::iterator b = functions.begin(), e = functions.end(); b != e; ++b) {
		(*b)->run();
	}
}

int main(int argc, char ** argv) {
	using namespace boost::program_options;

	size_t bits;
	const size_t DEFAULT_BITS = 32;

	size_t length;
#ifdef HASH_DEBUG
	const size_t DEFAULT_LENGTH = 10;
#else
	const size_t DEFAULT_LENGTH = 24;
#endif

	options_description opts("Universal systems test options.");
	opts.add_options()
		("bits", value<size_t>(&bits)->default_value(DEFAULT_BITS), "the number of bits used 32 or 64")
		("length", value<size_t>(&length)->default_value(DEFAULT_LENGTH), "the logarithm of number of times the function is called and the size of the table");
		
	variables_map vm;
	store(parse_command_line(argc, argv, opts), vm);
	notify(vm);
	
	if (bits == 32) {
		runTest<boost::uint32_t>(1 << length);
	} else if (bits == 64) {
		runTest<boost::uint64_t>(1 << length);
	} else {
		std::cerr << opts;
	}

	return 0;
}

