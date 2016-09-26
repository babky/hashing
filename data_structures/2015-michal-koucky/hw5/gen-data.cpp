#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

#include <cstring>
#include <limits>
#include <vector>
#include <set>
#include <iostream>

using namespace std;

// Check bits.
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
typedef size_t TT;
typedef uint32_t T;
#else
#define ENVIRONMENT32
typedef uint64_t TT;
typedef size_t T;
#endif
#endif

// Primes and types.
const T PRIME = std::numeric_limits<T>::max() - 4;

// Default lengths.
const size_t BASELEN = 512uL * 1024;

T nextrandom() {
	return (((T) rand()) * ((T) rand()) + (T) rand()
			+ ((T) rand() << sizeof(T) * 8 / 2)) ^ rand();
}

T nextrandom(T A, T B) {
	if (B <= A)
		return A;
	return A + (nextrandom() % (1 + B - A));
}

/**
 * Just shows the help screen.
 */
void print_help() {
	printf("Usage: ./gen-data -s|--seed <seed> [--short] <I|F> [C]\n");
}

const T MASK = (((T) 1) << (sizeof(T) * 8 - 1)) - 1;
void output_operation(char operation, T value) {
	printf("%c %u\n", operation, value);
}

class Generator {
public:
	virtual ~Generator() {

	}
	virtual T next() = 0;

	virtual bool has_next() {
		return true;
	}

};

class RandomProgressionGenerator: public Generator {
	T start, variance, increment;
public:
	RandomProgressionGenerator(T startArg, T varianceArg, T incrementArg) :
			start(startArg), variance(varianceArg), increment(incrementArg) {
	}

	virtual T next() {
		size_t v = nextrandom(start, start + variance);
		start += increment;
		return v & MASK;
	}
};

class IntervalGenerator: public Generator {
	T A, B;
public:
	IntervalGenerator(T aArg, T bArg) :
			A(aArg), B(bArg) {
		if (A > B) {
			T t = A;
			A = B;
			B = t;
		}
	}

	virtual T next() {
		return nextrandom(A, B) & MASK;
	}
};

class ProgressionGenerator: public Generator {
	T start, increment;
public:
	ProgressionGenerator(T startArg, T incrementArg) :
			start(startArg), increment(incrementArg) {
	}

	virtual T next() {
		size_t v = start;
		start += increment;
		return v & MASK;
	}
};

class HashedSequenceGenerator: public Generator {
	T universeMax;
	TT a, b;

public:
	HashedSequenceGenerator(T universeMaxArg) :
			universeMax(universeMaxArg), a(nextrandom()), b(nextrandom()) {

	}

	virtual T next() {
		return ((T) ((a * (TT) nextrandom(0, universeMax) + b) % PRIME)) & MASK;
	}

};

bool test_find(const T x) {
	return true;
}

class TestInsert : public Generator {
	set<T> data;
	set<T>::iterator it;

public:
	TestInsert() {
	}

	virtual ~TestInsert() {
	}

	bool operator()(const T e) {
		if (data.find(e) != data.end()) {
			return false;
		}

		data.insert(e);
		return true;
	}

	void reset() {
		it = data.begin();
	}

	virtual T next() {
		T v = (*it);
		++it;
		return v;
	}

	virtual bool has_next() {
		return it != data.end();
	}

};

template<typename test>
void generate(char operation, size_t M, vector<Generator *> g, test & t) {
	size_t s = 0;

	for (;;) {
		for (size_t i = 0; i < g.size(); ++i) {
			if (s == M) {
				return;
			}

			if (!g[i]->has_next()) {
				continue;
			}

			T e = g[i]->next();
			if (!t(e)) {
				continue;
			}

			output_operation(operation, e);

			++s;
		}
	}
}

int main(int argc, char** argv) {
	// The number of keys.
	size_t length = BASELEN;

	if (sizeof(TT) < 2 * sizeof(T)) {
		printf("Need the double arithmetics type to generate the data.\n");
		return 1;
	}

	int seed = -1;
	double insertion_prob = 0;
	size_t MAX_LOAD_FACTOR = 90;

	// Parse the args, somehow.
	for (int i = 1; i < argc; ++i) {
		if (!strcmp("-h", argv[i])) {
			print_help();
			return 0;
		}

		if (!strcmp("I", argv[i])) {
			insertion_prob = 0.8;
		} else if (!strcmp("F", argv[i])) {
			insertion_prob = 0.2;
		} else if (!strcmp("--short", argv[i])) {
			length = BASELEN / 256;
		} else if (!strcmp("--super-short", argv[i])) {
			length = BASELEN / 1024;
		} else if (!strcmp("-s", argv[i]) || !strcmp("--seed", argv[i])) {
			++i;
			seed = atoi(argv[i]);
		} else if (!strcmp("C", argv[i])) {
			MAX_LOAD_FACTOR = 55;
		} else {
			printf("Unknown argument %s \n.", argv[i]);
			print_help();
			return 1;
		}
	}

	if (insertion_prob == 0) {
		printf("The data variant I or F must be given!\n");
		print_help();
		return 1;
	}

	// Check the seed input.
	if (seed == -1) {
		printf("You must provide the seed!\n");
		print_help();
		return 1;
	}

	// Initialize the PRG.
	srand(seed);

	// Generate the data.

	// For each size of data, i.e. load factor generate RUNS runs.
	const size_t RUNS = 4;
	const size_t MIN_LOAD_FACTOR = 30;

	vector<Generator *> g;
	vector<Generator *> g2;
	g.push_back(new RandomProgressionGenerator(0, PRIME, RUNS));
	g.push_back(new HashedSequenceGenerator(length / 1024));
	g.push_back(new IntervalGenerator(0, PRIME));
	g.push_back(new ProgressionGenerator(0, 1));
	g.push_back(new ProgressionGenerator(0, length / RUNS));
	g.push_back(new HashedSequenceGenerator(length * 4));

	for (size_t l = MIN_LOAD_FACTOR; l <= MAX_LOAD_FACTOR; ++l) {
		for (size_t r = 0; r < RUNS; ++r) {
			size_t data_size = length * l / 100;
			size_t find_count = ((1 - insertion_prob) / insertion_prob) * data_size;

			// Table size.
			printf("# %lu %lu\n", data_size, length);

			// Generate the insertion set.
			TestInsert ti;
			generate('I', data_size, g, ti);

			g2.clear();
			g2.push_back(&ti);
			ti.reset();
			generate('I', data_size / 16, g2, test_find);

			ti.reset();
			// Generate the finds.
			g.push_back(&ti);
			g.push_back(&ti);
			g.push_back(&ti);
			g.push_back(&ti);
			generate('F', find_count, g, test_find);
			g.pop_back();
			g.pop_back();
			g.pop_back();
			g.pop_back();
		}
	}

	return 0;
}

