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

using namespace std;

// Check bits

#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
typedef unsigned __int128 TT;
#else
#define ENVIRONMENT32
typedef uint64_t TT;
#endif
#endif

// Primes and types;
typedef size_t T;
#ifdef ENVIRONMENT32
const TT PRIME = std::numeric_limits<T>::max() - 4;
#else
const TT PRIME = std::numeric_limits<T>::max() - 58;
#endif

// Default lengths.
#ifdef ENVIRONMENT32
const size_t BASELEN = 1uL * 1024 * 1024 * 1024 / (sizeof(T) * 2); // 2 G of data.
#else
const size_t BASELEN = 32uL * 1024 * 1024 * 1024 / (sizeof(T) * 2); // cca 64 G of data.
#endif
const size_t TESTLEN = 128;

T nextrandom() {
	return (((T) rand()) * ((T) rand()) + (T) rand() + ((T) rand() << sizeof(T) * 8 / 2)) ^ rand();
}

T nextrandom(T A, T B) {
	if (B <= A)
		return A;
	return A + (nextrandom() % (1 + B - A));
}

void print_help() {
	printf("Usage: ./gen-data [-s|--seed <seed>] [--test] [--short] [--super-short]\n");
}

class Generator {
public:
	virtual ~Generator() {

	}
	virtual void next() = 0;
};

class RandomProgressionGenerator : public Generator {
	T start, variance, increment;
public:
	RandomProgressionGenerator(T startArg, T varianceArg, T incrementArg):
		start(startArg),
		variance(varianceArg),
		increment(incrementArg) {
	}

	virtual void next() {
		printf("%lu %lu\n", nextrandom(start, start + variance), nextrandom());
		start += increment;
	}
};

class IntervalGenerator : public Generator {
	T A, B;
public:
	IntervalGenerator(T aArg, T bArg):
		A(aArg),
		B(bArg) {
		if (A > B) {
			T t = A;
			A = B;
			B = t;
		}
	}

	virtual void next() {
		printf("%lu %lu\n", nextrandom(A, B), nextrandom());
	}
};

class ProgressionGenerator : public Generator {
	T start, increment;
public:
	ProgressionGenerator(T startArg, T incrementArg):
		start(startArg),
		increment(incrementArg) {
	}

	virtual void next() {
		printf("%lu %lu\n", start, increment);
		start += increment;
	}
};

class HashedSequenceGenerator : public Generator {
	T universeMax;
	TT a, b;

public:
	HashedSequenceGenerator(T universeMaxArg):
		universeMax(universeMaxArg),
		a(nextrandom()),
		b(nextrandom()) {

	}

	virtual void next() {
		printf("%lu %lu\n", (T) ((a * (TT) nextrandom(0, universeMax) + b) % PRIME % universeMax), nextrandom());
	}
};

int main(int argc, char** argv) {
	// The number of keys.
	T length = BASELEN;

	printf("%lu", sizeof(TT));
	if (sizeof(TT) < 2 * sizeof(T)) {
		printf("Need the double arithmetics type to generate the data.\n");
		return 1;
	}

	int seed = -1;

	// Parse the args, somehow.
	for (int i = 1; i < argc; ++i) {
		if (!strcmp("-h", argv[i])) {
			print_help();
			return 0;
		}

		if (!strcmp("--short", argv[i])) {
			length = BASELEN / 16;
		}

		if (!strcmp("--super-short", argv[i])) {
			length = BASELEN / 256;
		}

		if (!strcmp("--test", argv[i])) {
			length = TESTLEN;
		}

		if (!strcmp("-s", argv[i]) || !strcmp("--seed", argv[i])) {
			++i;
			seed = atoi(argv[i]);
		}
	}

	// Check the seed input.
	if (seed == -1) {
		printf("You must provide the seed!\n");
		print_help();
		return 1;
	}

	// Initialize the PRG.
	srand(seed);

	// Set up the generated set.
	vector<Generator *> generators;
	HashedSequenceGenerator * hg = new HashedSequenceGenerator(length / 16);
	generators.push_back(hg);
	generators.push_back(hg);
	generators.push_back(new ProgressionGenerator(nextrandom(), nextrandom()));
	generators.push_back(hg);
	generators.push_back(hg);
	generators.push_back(new RandomProgressionGenerator(nextrandom(), 170, 10000));
	generators.push_back(hg);
	generators.push_back(hg);
	generators.push_back(new IntervalGenerator(170, 10000));
	generators.push_back(hg);
	generators.push_back(hg);
	generators.push_back(new IntervalGenerator(0, length));
	generators.push_back(hg);
	generators.push_back(hg);
	generators.push_back(new ProgressionGenerator(nextrandom(), nextrandom()));
	generators.push_back(hg);

	// Generate the data.
	for (T i = 0; i < length; ++i) {
		generators[i % generators.size()]->next();
	}

	return 0;
}

