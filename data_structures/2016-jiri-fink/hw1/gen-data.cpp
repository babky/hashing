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

// Check bits.
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
typedef unsigned __int128 TT;
#else
#define ENVIRONMENT32
typedef uint64_t TT;
#endif
#endif

// Primes and types.
typedef size_t T;
#ifdef ENVIRONMENT32
const TT PRIME = std::numeric_limits<T>::max() - 4;
#else
const TT PRIME = std::numeric_limits<T>::max() - 58;
#endif

// Default lengths.
#ifdef ENVIRONMENT32
const size_t BASELEN = 1uL * 1024 * 1024 * 1024 / sizeof(T); // 2 G of data.
#else
const size_t BASELEN = 19uL * 1024 * 1024 * 1024 / sizeof(T); // cca 48 G of data.
#endif
const size_t TESTLEN = 4096;

T nextrandom() {
	return (((T) rand()) * ((T) rand()) + (T) rand() + ((T) rand() << sizeof(T) * 8 / 2)) ^ rand();
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
	printf("Usage: ./gen-data [-s|--seed <seed>] [--short] \n");
}

const T MASK = (((T) 1) << (sizeof(T) * 8 - 1)) - 1;
void output_key_value(T key) {
	printf("%lu\n", key & MASK);
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
		output_key_value(nextrandom(start, start + variance));
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
		output_key_value(nextrandom(A, B));
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
		output_key_value(start);
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
		output_key_value((T) ((a * (TT) nextrandom(0, universeMax) + b) % PRIME));
	}
};

void generate_chunk_run(T chunk_run_lengh, T chunk_length, T overall_chunk_length, std::vector<HashedSequenceGenerator *> & hgs) {
	std::vector<Generator *> generators;

	size_t hg_index = 0;
	for (T i = 0; i < chunk_run_lengh; ++i) {
		if (i % chunk_length == 0) {
			// Delete the old generators.
			for (size_t j = 0; j < generators.size(); ++j) {
				// Do not delete the hg generators.
				if (generators[j] == hgs[hg_index - 1]) {
					continue;
				}

				delete generators[j];
			}
			generators.clear();

			// Get the chunk generator.
			HashedSequenceGenerator * hg;
			if (hg_index < hgs.size()) {
				hg = hgs[hg_index];
			} else {
				hg = new HashedSequenceGenerator(3 * overall_chunk_length / 8);
				hgs.push_back(hg);
			}
			++hg_index;

			// Init the generators.
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
			generators.push_back(new IntervalGenerator(0, overall_chunk_length));
			generators.push_back(hg);
			generators.push_back(hg);
			generators.push_back(new ProgressionGenerator(nextrandom(), nextrandom()));
			generators.push_back(hg);
		}

		// Generate the data.
		generators[i % generators.size()]->next();
	}
}

int main(int argc, char** argv) {
	// The number of keys.
	T length = BASELEN;

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
		} else if (!strcmp("--super-short", argv[i])) {
			length = BASELEN / 256;
		} else if (!strcmp("--test", argv[i])) {
			length = TESTLEN;
		} else if (!strcmp("-s", argv[i]) || !strcmp("--seed", argv[i])) {
			++i;
			seed = atoi(argv[i]);
		} else {
			printf("Unknown argument %s \n.", argv[i]);
			print_help();
			return 1;
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

	// For full size data we choose.
	// 2 GB of memory = 134 217 728 of elements
	// The size of the chunk is thus - 1/24 of overall data size.
	size_t chunk_length = length / 24;
	size_t first_chunk_length = 3 * length / 24 / 4;
	size_t second_chunk_length = 1 * length / 24 / 4;

	// For shorter size data we choose the sizes of chunks proportionally.

	// We chunk the data by 134 217 728 of elements.
	// In each chunk we have the universe of size ~ 134 217 728 / 2.

	// In such a universe we choose the elements and hash them (HashedSequenceGenerator).
	// Then we also add some elements into these chunks - progressions.

	// The overall created data looks like:
	//  - chunks
	//  - length / 32 -> ascending progression
	//  - length / 32 -> descending progression
	//  - chunks

	T first_chunk_run_length = 3 * (length - 2 * length / 32) / 4;
	T second_chunk_run_length = 1 * (length - 2 * length / 32) / 4;
	std::vector<HashedSequenceGenerator *> hgs;

	// Generate the data.

	// First chunks.
	generate_chunk_run(first_chunk_run_length, first_chunk_length, chunk_length, hgs);

	// Progression.
	ProgressionGenerator pg_asc(nextrandom(), nextrandom() % (1 << 30));
	for (T i = 0; i < length / 32; ++i) {
		pg_asc.next();
	}

	// Progression.
	ProgressionGenerator pg_desc(nextrandom() | (((T) 1) << (sizeof(T) * 8 - 2)), -(nextrandom() % (1 << 30)));
	for (T i = 0; i < length / 32; ++i) {
		pg_desc.next();
	}

	// Finish the chunks.
	generate_chunk_run(second_chunk_run_length, second_chunk_length, chunk_length, hgs);

	return 0;
}

