#define HASH_DEBUG

#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <algorithm>
#include "storages/collision_count_storage.h"
#include <boost/multiprecision/cpp_int.hpp>
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

	size_t iterations = 0;

	Function f;
	while (iterator.hasNext()) {
		f = iterator.next();
		collisions += collide(f, elements) ? 1 : 0;
		++iterations;
	}

	return collisions;
}

void print_out_matrix(const boost::numeric::ublas::matrix<size_t> & m, std::ostream & out) {
	for (size_t i = 0; i < m.size1(); ++i) {
		for (size_t j = 0; j < m.size2(); ++j) {
			out << m(i, j) << " ";
		}
		out << "\n";
	}
}

template<typename T>
struct BinaryOperations {

	static size_t divide(size_t dividee, size_t divisor) {
		simple_assert(divisor != 0, "Divisor can not be null.");
		return dividee;
	}

	static size_t multiply(size_t x, size_t y) {
		return x & y;
	}

	static size_t subtract(size_t x, size_t y) {
		return x ^ y;
	}

};

template<typename T, template<typename T> class operations>
void gauss_elimination_desctructive(boost::numeric::ublas::matrix<T> & m) {
	// Compute the Gauss elimination of the matrix.
	size_t col = 0;
	for (size_t i = 0; i < m.size1(); ++i) {
		for (; col < m.size2() && m(i, col) == 0; ++col) {
			// Find the row where m[j, col] != 0.
			size_t j = i + 1;
			for (; j < m.size1(); ++j) {
				if (m(j, col) != 0) {
					break;
				}
			}

			if (j != m.size1()) {
				// Swap rows i and j.
				for (size_t k = col; k < m.size2(); ++k) {
					std::swap(m(i, k), m(j, k));
				}

				break;
			}
		}

		if (col == m.size2()) {
			break;
		}

		for (size_t j = i + 1; j < m.size1(); ++j) {
			if (m(j, col) != 0) {
				// Add row i to row j.
				size_t factor = operations<T>::divide(m(j, col), m(i, col));

				for (size_t k = col; k < m.size2(); ++k) {
					m(j, k) = operations<T>::subtract(m(j, k), operations<T>::multiply(factor, m(i, k)));
				}
			}
		}
	}
}

template<typename T, template<typename T> class operations>
boost::numeric::ublas::matrix<T> gauss_elimination(const boost::numeric::ublas::matrix<T> & m) {
	boost::numeric::ublas::matrix<T> result = m;
	gauss_elimination_desctructive(result);
	return result;
}

template<typename T>
size_t compute_rank_destructive(boost::numeric::ublas::matrix<T> & m) {
	gauss_elimination_desctructive<size_t, BinaryOperations>(m);
	size_t rank = 0;
	for (size_t j = 0, i = 0; i < m.size1(); ++i) {
		for (j = 0; j < m.size2(); ++j) {
			if (m(i, j) != 0) {
				++rank;
				break;
			}
		}

		if (j == m.size2()) {
			break;
		}
	}

	return rank;
}

template<typename T>
size_t compute_rank(const boost::numeric::ublas::matrix<T> & m) {
	boost::numeric::ublas::matrix<T> copy = m;
	return compute_rank_destructive(copy);
}

void write_vector_as_rows_in_matrix(boost::numeric::ublas::matrix<size_t> & m, size_t vectorNo, size_t vector,
		size_t universeBitSize, size_t tableBitSize) {
	for (size_t row = 0; row < tableBitSize; ++row) {
		size_t mask = 1;
		for (size_t i = 1; i <= universeBitSize; ++i) {
			m(vectorNo * tableBitSize + row, row * universeBitSize + universeBitSize - i) = (vector & mask) >> (i - 1);
			mask <<= 1;
		}
	}
}

void write_vector_as_col_in_matrix(boost::numeric::ublas::matrix<size_t> & m, size_t vector, size_t column,
		size_t vectorBitSize, size_t repeats) {
	for (size_t repeat = 0; repeat < repeats; ++repeat) {
		size_t mask = 1;
		for (size_t i = 0; i < vectorBitSize ; ++i) {
			m(repeat * vectorBitSize + i, column) = (vector & mask) >> i;
			mask <<= 1;
		}
	}

}

/*
A is a matrix. The vectors r, s, t are supposed to collide into y.

Ar = y
As = y
At = y

a11r1 + a12r2 + a13r3 + a14r4                                                             = y1
                              a21r1 + a22r2 + a23r3 + a24r4                               = y2
                                                            a31r1 + a32r2 + a33r3 + a34r4 = y3
a11s1 + a12s2 + a13s3 + a14s4                                                             = y1
                              a21s1 + a22s2 + a23s3 + a24s4                               = y2
                                                            a31s1 + a32s2 + a33s3 + a34s4 = y3

*/

typedef boost::multiprecision::number<boost::multiprecision::cpp_int_backend<>, boost::multiprecision::et_off> number;

number compute_binary_power(size_t exponent) {
	number x = 1;
	x <<= exponent;
	return x;
}

number collision_count_linear_map(const ElementVector & elements, size_t universeBitSize, size_t tableSize) {
	size_t tableBitSize = Hash::Math::log2exact(tableSize);
	size_t matrixCols = universeBitSize * tableBitSize;
	size_t matrixRows = tableBitSize * elements.size();

	boost::numeric::ublas::matrix<size_t> matrix(matrixRows, matrixCols, 0);
	boost::numeric::ublas::matrix<size_t> extendedMatrix(matrixRows, matrixCols + 1, 0);

	number collisionCount = 0;

	size_t vectorNo = 0;
	// Create the matrix and the extended matrix.
	for (ElementVector::const_iterator i = elements.begin(); i != elements.end(); ++i) {
		write_vector_as_rows_in_matrix(matrix, vectorNo, *i, universeBitSize, tableBitSize);
		write_vector_as_rows_in_matrix(extendedMatrix, vectorNo++, *i, universeBitSize, tableBitSize);
	}

	size_t rank = compute_rank_destructive(matrix);

	number kernelSize = compute_binary_power(matrixCols - rank);

	for (size_t y = 0; y < tableSize; ++y){
		write_vector_as_col_in_matrix(extendedMatrix, y, matrixCols, tableBitSize, elements.size());
		size_t extendedRank = compute_rank(extendedMatrix);
		if (rank == extendedRank) {
			collisionCount += kernelSize;
		}
	}

	return collisionCount;
}

int main(int argc, char ** argv) {
	using namespace std;
	using namespace boost::program_options;

	typedef UniversalFunctionCWLF<T, CollisionCountStorage<T, EqualityComparer<T>, size_t> > CWLFFunction;
	typedef MultiplyShiftSystem<T, CollisionCountStorage<T, EqualityComparer<T>, size_t> > MultiplyShiftFunction;
	typedef UniversalFunctionLinearMap<T, CollisionCountStorage<T, EqualityComparer<T>, size_t> > LinearMapFunction;

	size_t DEFAULT_TABLE_SIZE = 32;
	size_t DEFAULT_BITS = 11;
	size_t DEFAULT_RUNS = 1;
	size_t DEFAULT_X = 2;
	size_t DEFAULT_Y = 3;
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
	v.push_back(x);

	const size_t tableBitSize = Hash::Math::log2exact(tableSize);
	size_t cwlfColls = 0;
	size_t msColls = 0;
	number lmColls = 0;
	number lfSystemSize = compute_binary_power(tableBitSize * bits);
	size_t lmCollsSampled = 0;
	size_t lmCollsComplete = 0;
	size_t randomRuns = 1024 * tableSize;

	for (size_t i = 0; i != universumSize; ++i) {
		if (std::find(v.begin(), v.end(), i) != v.end()) {
			continue;
		}

		v[v.size() - 1] = i;

		cwlfColls = collision_count<CWLFFunction>(v, CompleteFunctionIterator<CWLFFunction>(primes[bits], tableSize));
		msColls = collision_count<MultiplyShiftFunction>(v, CompleteFunctionIterator<MultiplyShiftFunction>(universumMax, tableSize));
		lmColls = collision_count_linear_map(v, bits, tableSize);
		lmCollsSampled = collision_count<LinearMapFunction>(v, RandomFunctionIterator<LinearMapFunction>(randomRuns, tableSize));
		// lmCollsComplete = collision_count<LinearMapFunction>(v, CompleteFunctionIterator<LinearMapFunction>(universumMax, tableSize));

		std::cout
			<< i << ","
			<< cwlfColls << "," << cwlfColls / (static_cast<double>(primes[bits]) * primes[bits]) << ","
			<< msColls << "," << msColls / (static_cast<double>(universumMax / 2)) << ","
			<< lmColls << "," << boost::multiprecision::cpp_rational(lmColls, lfSystemSize).convert_to<double>() << ","
			// << lmCollsComplete << "," << lmCollsComplete / pow(2, tableBitSize * bits) << ","
			<< lmCollsSampled << "," << lmCollsSampled / static_cast<double>(randomRuns)
			<< std::endl;
	}


	return 0;
}


