#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "utils/boost_include.h"
#include <boost/program_options.hpp>
#include "systems/uniform/dietzfelbinger_woelfel.h"
#include "systems/bit_string_system.h"
#include "systems/polynomial_system.h"
#include "systems/linear_map_system.h"
#include "systems/cwlf_system.h"
#include "systems/multiply_shift_system.h"
#include "systems/cwlf_exponential_system.h"
#include "systems/tabulation_system.h"
#include "systems/polynomial_system.h"
#include "systems/tr1_function.h"
#include "systems/identity_function.h"
#include "utils/time_vector.h"

using namespace std;
using namespace boost::posix_time;
using namespace Hash::Utils;

struct Test {
	Test(string aName, size_t aLength, size_t aRepeats):
		name(aName),
		length(aLength),
		repeats(aRepeats)
	{
	}

	virtual ~Test(void) {
	}

	const string & getName(void) const {
		return name;
	}

	virtual void run(void) = 0;

	TimeVector & getTimes(void) {
		return times;
	}

protected:
	string name;
	size_t length;
	size_t repeats;
	TimeVector times;
};

template<class F, typename T>
struct FunctionTest : public Test {

	FunctionTest(string aName, size_t aLength, size_t aRepeats):
		Test(aName, aLength, aRepeats)
	{
	}

	virtual void run() {
		cout << "Running " << name << " of length " << length << ".\n";

		function.setTableSize(length);

		ptime start, finish;
		size_t hash = 0xdead;
		for (size_t j = 0; j < repeats; ++j) {
			start = microsec_clock::local_time();
			function.reset();
			hash = 0xdead;

			for (size_t i = 0; i < length; ++i) {
				hash ^= function.hash(i);
			}

			if (hash == 0) {
				cout << "Almost impossible :)!\n";
			}

			finish = microsec_clock::local_time();
			times.add(finish - start);
		}

	}

private:
	F function;
};


class CompleteTest {
public:
	typedef vector<Test *> FunctionTestVector;

	CompleteTest(size_t aLength, size_t aRepeats):
		length(aLength),
		repeats(aRepeats) {
	}

	template<typename T>
	void runTest(void) {
		typedef size_t TestStorage;
		using namespace Hash::Systems;

		functions.push_back(new FunctionTest<IdentityFunction<T, TestStorage>, T>("ID", length, repeats));
		functions.push_back(new FunctionTest<Tr1Function<T, TestStorage>, T>("TR1", length, repeats));
		functions.push_back(new FunctionTest<UniversalFunctionCWLF<T, TestStorage>, T>("CWLF", length, repeats));
		functions.push_back(new FunctionTest<CWLFExponentialSystem<T, TestStorage>, T>("CWLFExp", length, repeats));
		functions.push_back(new FunctionTest<BitStringFunction<T, TestStorage>, T>("BitString", length, repeats));
		functions.push_back(new FunctionTest<TabulationFunction<T, TestStorage>, T>("Tabulation", length, repeats));
		functions.push_back(new FunctionTest<UniversalFunctionLinearMap<T, TestStorage>, T>("LinearMap", length, repeats));
		functions.push_back(new FunctionTest<PolynomialSystem<T, TestStorage>, T>("Polynomial - deg 2", length, repeats));
		functions.push_back(new FunctionTest<PolynomialSystem4<T, TestStorage>, T>("Polynomial - deg 4", length, repeats));
		functions.push_back(new FunctionTest<Uniform::DietzfelbingerWoelfel<T, TestStorage, Hash::Systems::PolynomialSystem4>, T>("DW - deg 4", length, repeats));
		functions.push_back(new FunctionTest<MultiplyShiftSystem<T, TestStorage>, T>("MultiplyShift", length, repeats));

		for (FunctionTestVector::iterator b = functions.begin(), e = functions.end(); b != e; ++b) {
			(*b)->run();
		}
	}

	size_t getLength(void) const {
		return length;
	}

	size_t getRepeats(void) const {
		return repeats;
	}

	FunctionTestVector & getFunctions(void) {
		return functions;
	}

protected:
	FunctionTestVector functions;

	size_t length;
	size_t repeats;
};

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

	size_t repeats;
	const size_t DEFAULT_REPEATS = 32;

	size_t step;
	const size_t DEFAULT_STEP = 1;

	size_t steps;
	const size_t DEFAULT_STEPS = 0;

	string output;
	string DEFAULT_OUTPUT = "";

	bool help;

	options_description opts("Universal systems test options.");
	opts.add_options()
		("bits", value<size_t>(&bits)->default_value(DEFAULT_BITS), "the number of bits used 32 or 64")
		("length", value<size_t>(&length)->default_value(DEFAULT_LENGTH), "the logarithm of number of times the function is called and the size of the table")
		("repeats", value<size_t>(&repeats)->default_value(DEFAULT_REPEATS), "the number of repetitions")
		("step", value<size_t>(&step)->default_value(DEFAULT_STEP), "the iteration step")
		("steps", value<size_t>(&steps)->default_value(DEFAULT_STEPS), "the number of steps")
		("output", value<string>(&output)->default_value(DEFAULT_OUTPUT), "output file")
		("help", "prints this help.");

	variables_map vm;
	store(parse_command_line(argc, argv, opts), vm);
	notify(vm);

	if (vm.count("help")) {
		cout << opts;
		return 0;
	}

	typedef vector<CompleteTest> TestVector;
	TestVector v;

	for (size_t s = 0; s < steps; ++s) {
		CompleteTest t(1 << (length + s * step), repeats);

		if (bits == 32) {
			t.runTest<boost::uint32_t>();
		} else if (bits == 64) {
			t.runTest<boost::uint64_t>();
/*#ifdef __GNUC__ TODO: Not working in gcc 4.7.
		} else if (bits == 128) {
			t.runTest<__uint128_t>();
#endif*/
		} else {
			std::cerr << opts;
			return 1;
		}

		v.push_back(t);
	}

	ofstream fout;
	if (output != "") {
		fout.open(output.c_str());
	}

	ostream & out = output != "" ? fout : cout;
	for (TestVector::iterator b = v.begin(), e = v.end(); b != e; ++b) {
		out << "TEST: length = " << setw(10) << b->getLength() << ", repeats = " << setw(2) << b->getRepeats() << "\n";
		for (CompleteTest::FunctionTestVector::iterator fb = b->getFunctions().begin(), fe = b->getFunctions().end(); fe != fb; ++fb) {
			out << setw(20) << (*fb)->getName() << " " << setw(6) << right << (*fb)->getTimes().getAverageTime().total_milliseconds() << " [(+/-) " << setw(10) << fixed << setprecision(3) << (*fb)->getTimes().getMillisVariance() << "] ms\n";
		}
		out << "\n";
	}

	if (output != "") {
		out << "\n\n---PRINTING WHOLE DATA SET---\n";
		for (TestVector::iterator b = v.begin(), e = v.end(); b != e; ++b) {
			for (CompleteTest::FunctionTestVector::iterator fb = b->getFunctions().begin(), fe = b->getFunctions().end(); fe != fb; ++fb) {
				out << setw(20) << (*fb)->getName() << ", length = " << setw(10) << b->getLength() << ", repeats = " << setw(2) << b->getRepeats() << ": ";
				out << (*fb)->getTimes() << "\n";
			}

		}

		fout.close();
	}

	return 0;
}

