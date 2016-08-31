#include <iostream>
#include "storages/collision_count_storage.h"
#include "systems/cwlf_system.h"
#include "systems/multiply_shift_system.h"
#include "systems/linear_map_system.h"
#include "utils/equality_comparer.h"
#ifdef BOOST_MSVC
	#pragma warning(disable: 4512)
#endif
#include <boost/program_options.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512)
#endif


/**
 * Computes the PDF for linear functions
 *  - cwlf
 *  - multiply-shift
 *  - matrix multiplication (linear function/transformation)
 */

using namespace Hash;
using namespace Hash::Storages;
using namespace Hash::Systems;
using namespace Hash::Utils;

typedef std::vector<size_t> ElementVector;
typedef size_t T;

template<class Function>
bool collide(Function & f, const ElementVector & elements) {
	ElementVector::const_iterator b = elements.begin();
	size_t y = f.hash(*b);

	for (++b; b != elements.end(); ++b) {
		if (y != f.hash(*b)) {
			return false;
		}
	}

	return true;
}

template <class Function>
class FunctionIterator {
public:
	virtual ~FunctionIterator(void) {
	}

	virtual bool hasNext(void) = 0;
	virtual Function next() = 0;
};


template<class Function>
class CompleteFunctionIterator : public FunctionIterator<Function> {
public:
	CompleteFunctionIterator(std::size_t universumMax, std::size_t tableSize):
		g(universumMax, tableSize) {
	}

	virtual ~CompleteFunctionIterator(void) {
	}

	virtual bool hasNext(void) {
		return g.hasNext();
	}

	virtual	Function next() {
		return g.next();
	}

private:
	typename Function::Generator g;
};

template<typename T>
class CompleteFunctionIterator<UniversalFunctionCWLF<T, CollisionCountStorage<T, EqualityComparer<T>, size_t> > > : public FunctionIterator<UniversalFunctionCWLF<T, CollisionCountStorage<T, EqualityComparer<T>, size_t> > > {
public:
	typedef UniversalFunctionCWLF<T, CollisionCountStorage<T, EqualityComparer<T>, size_t> > Function;

	CompleteFunctionIterator(std::size_t prime, std::size_t tableSize):
		g(prime, tableSize, false) {
	}

	virtual ~CompleteFunctionIterator(void) {
	}

	virtual bool hasNext(void) {
		return g.hasNext();
	}

	virtual	Function next() {
		return g.next();
	}

private:
	typename Function::Generator g;
};

template<class Function>
class RandomFunctionIterator : public FunctionIterator<Function> {
public:
	RandomFunctionIterator(std::size_t aRuns, std::size_t aTableSize):
		runs(aRuns),
		run(0),
		tableSize(aTableSize) {
	}

	virtual ~RandomFunctionIterator(void) {
	}

	virtual bool hasNext(void) {
		return run < runs;
	}

	virtual	Function next() {
		Function f;
		f.setTableSize(tableSize);
		f.reset();
		++run;
		return f;
	}

private:
	std::size_t runs;
	std::size_t run;
	std::size_t tableSize;
};

template<class Function, template <class> class FunctionIterator>
size_t collision_count(const ElementVector & elements, FunctionIterator<Function> iterator) {
	size_t collisions = 0;

	for (Function f; iterator.hasNext(); f = iterator.next()) {
		collisions += collide(f, elements) ? 1 : 0;
	}

	return collisions;
}

int main(int argc, char ** argv) {
	using namespace std;
	using namespace boost::program_options;

	typedef UniversalFunctionCWLF<T, CollisionCountStorage<T, EqualityComparer<T>, size_t> > CWLFFunction;
	typedef MultiplyShiftSystem<T, CollisionCountStorage<T, EqualityComparer<T>, size_t> > MultiplyShiftFunction;
	typedef UniversalFunctionLinearMap<T, CollisionCountStorage<T, EqualityComparer<T>, size_t> > LinearMapFunction;

	size_t DEFAULT_TABLE_SIZE = 32;
	size_t DEFAULT_BITS = 10;
	size_t DEFAULT_RUNS = 1;
	size_t DEFAULT_X = 0;
	size_t DEFAULT_Y = 1;
	size_t tableSize;
	size_t bits;
	size_t runs;
	size_t x = DEFAULT_X;
	size_t y = DEFAULT_Y;

	options_description optsDesc("Probability distribution computation.");
	optsDesc.add_options()
		("help", "prints this help message")
		("m", value<size_t>(&tableSize)->default_value(DEFAULT_TABLE_SIZE), "The size of the table.")
		("u", value<size_t>(&bits)->default_value(DEFAULT_BITS), "The number of bits.")
		("x", value<size_t>(&x)->default_value(DEFAULT_X), "The first fixed element.")
		("y", value<size_t>(&y)->default_value(DEFAULT_Y), "The second fixed element.")
		("runs", value<size_t>(&runs)->default_value(DEFAULT_RUNS), "The number of runs.");

	variables_map vm;
	try {
		store(parse_command_line(argc, argv, optsDesc), vm);
		notify(vm);
	} catch (std::exception & e) {
		std::cerr << e.what() << std::endl;
		throw e;
	}

	if (vm.count("help")) {
		cout << optsDesc;
		return 0;
	}

	size_t universumSize = 1 << bits;
	size_t universumMax = universumSize - 1;

	std::vector<size_t> primes {
		0, 0, 3, 7, 13, 31, 61, 127,
		(1 << 8) - 5,
		(1 << 9) - 3,
		(1 << 10) - 3,
		(1 << 11) - 9,
		(1 << 12) - 3,
		(1 << 13) - 1,
		(1 << 14) - 3,
		(1 << 15) - 19,
		(1 << 16) - 15,
		(1 << 17) - 1,
		(1 << 18) - 5,
		(1 << 19) - 1,
		(1 << 20) - 3,
		(1 << 21) - 9,
		(1 << 22) - 3,
		(1 << 23) - 15,
		(1 << 24) - 3,
		(1 << 25) - 39,
		(1 << 26) - 5,
		(1 << 27) - 39,
		(1 << 28) - 57,
		(1 << 29) - 3,
		(1 << 30) - 35,
		2147483647,
		4294967291
	};

	ElementVector v;
	v.push_back(x);
	v.push_back(y);
	v.push_back(0);
	for (size_t i = 2; i != universumSize; ++i) {
		v[v.size() - 1] = i;

		size_t cwlfColls = collision_count<CWLFFunction>(v, CompleteFunctionIterator<CWLFFunction>(primes[bits], tableSize));
		size_t msColls = collision_count<MultiplyShiftFunction>(v, CompleteFunctionIterator<MultiplyShiftFunction>(universumMax, tableSize));
		size_t lmColls = collision_count<LinearMapFunction>(v, RandomFunctionIterator<LinearMapFunction>(universumMax * universumMax * universumMax, tableSize));

		std::cout << i << "," << cwlfColls << "," << msColls << "," << lmColls << "\n";
	}


	return 0;
}
