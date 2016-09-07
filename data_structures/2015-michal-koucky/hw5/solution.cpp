/*
 * solution.cpp
 *
 * Implements:
 *  - various hash functions (H_mult by Dietzfelbinger, tabulation and simple linear functions)
 *  - various hashing schemes - linear probing, two-choices with chaining, cuckoo
 *  - measures - number of operations per insert/find and running times
 *
 *  Created on: Dec 14, 2015
 *	  Author: Martin Babka
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
typedef unsigned __int128 double_size_t;
#else
#define ENVIRONMENT32
typedef uint64_t double_size_t;
#endif
#endif

const size_t SIZE_T_BITS = sizeof(size_t) * 8;

// #define LOG_SUPPORT

class Log {
#ifdef LOG_SUPPORT
	bool output;
#endif

public:
	bool is_outputting() {
#ifdef LOG_SUPPORT
		return output;
#else
		return false;
#endif

	}

public:
	Log(bool show) {
#ifdef LOG_SUPPORT
		output = show;
#endif
	}
};

#ifdef LOG_SUPPORT

template<typename T>
Log & operator <<(Log & log, const T & x) {
	if (log.is_outputting()) {
		std::cerr << x;
		std::cerr.flush();
	}

	return log;
}

#else

template<typename T>
Log & operator <<(Log & log, const T & x) {
}

#endif

Log logger(true);
Log logger_trace(false);

struct Stats {
	std::size_t size = 0;

	std::size_t data_size = 0;
	std::size_t inserts = 0;
	std::size_t finds = 0;

	std::size_t insert_swaps = 0;
	std::size_t insert_find_operations = 0;
	std::size_t rehashes = 0;

	std::size_t find_operations = 0;

	std::size_t running_time = 0;
	std::clock_t clock_begin;

	void start() {
		clock_begin = clock();
	}

	void finish() {
		running_time = (((double) (clock() - clock_begin)) * 1000
				/ CLOCKS_PER_SEC);
	}

	void add(const Stats & r) {
		insert_swaps += r.insert_swaps;
		insert_find_operations += r.insert_find_operations;
		rehashes += r.rehashes;
		find_operations += r.find_operations;
		running_time += r.running_time;
	}

	void print(std::ostream & out) {
		out << "Size: " << size << "\n";
		out << "DataSize: " << data_size << "\n";
		out << "Inserts: " << inserts << "\n";
		out << "Finds: " << finds << "\n";

		out << "InsertSwaps: " << insert_swaps << "\n";
		out << "InsertFindOperations: " << insert_find_operations << "\n";
		out << "Rehashes: " << rehashes << "\n";

		out << "FindOperations: " << find_operations << "\n";

		out << "Time: " << running_time << "\n";
		out << "\n";
	}

	void printCsv(std::ostream & out) {
		out
			<< size << ", "
			<< data_size << ", "
			<< inserts << ", "
			<< finds << ", "
			<< insert_swaps << ", "
			<< insert_find_operations << ", "
			<< rehashes << ", "
			<< find_operations << ", "
			<< running_time << "\n";
	}

};

const size_t MAX_SIZE_T = -1;

size_t log2(size_t m) {
	size_t l = 0;
	while (m > 1) {
		++l;
		m >>= 1;
	}
	return l;
}

void init_rng() {
	srand(time(0));
}

size_t nextrandom() {
	return (((size_t) rand()) * ((size_t) rand()) + (size_t) rand()
			+ ((size_t) rand() << SIZE_T_BITS / 2)) ^ rand();
}

size_t nextrandom(size_t min, size_t max) {
	return min + (nextrandom() % (max - min));
}
namespace std {
template<typename T>
std::string to_string(T value) {
	std::ostringstream os;
	os << value;
	return os.str();
}
}

class MultiplicationFunction {
	size_t a;

public:
	MultiplicationFunction() :
			a(nextrandom(0, MAX_SIZE_T - 1) + 1) {
	}

	size_t compute(size_t x, size_t m) {
		size_t y = a * x;
		y = y >> (SIZE_T_BITS - log2(m));
		logger_trace << m << "\n";
		logger_trace << y << "\n";
		return y;
	}

};

class TabulationFunction {
	size_t ** table;
	size_t char_length = 4;
	size_t no_chars;
	size_t mask;
	size_t initial_shift;

public:
	TabulationFunction() {
		no_chars = SIZE_T_BITS / char_length;
		initial_shift = SIZE_T_BITS - char_length;

		mask = (std::numeric_limits<size_t>::max() >> initial_shift)
				<< initial_shift;
		table = new size_t *[no_chars];

		for (size_t i = 0; i < no_chars; ++i) {
			size_t char_combinations = 1 << char_length;
			table[i] = new size_t[char_combinations];

			for (size_t j = 0; j < char_combinations; ++j) {
				table[i][j] = nextrandom();
			}
		}

		logger << "Initialized TabulationFunction, chars: " << no_chars
				<< ".\n";
	}

	TabulationFunction(const TabulationFunction & r) {
		no_chars = r.no_chars;
		initial_shift = r.initial_shift;

		mask = r.mask;
		table = new size_t *[no_chars];

		for (size_t i = 0; i < no_chars; ++i) {
			size_t char_combinations = 1 << char_length;
			table[i] = new size_t[char_combinations];

			for (size_t j = 0; j < char_combinations; ++j) {
				table[i][j] = r.table[i][j];
			}
		}
	}

	TabulationFunction & operator=(const TabulationFunction & r) {
		TabulationFunction tmp(r);
		swap(tmp);
		return *this;
	}

	void swap(TabulationFunction & r) {
		std::swap(char_length, r.char_length);
		std::swap(table, r.table);
		std::swap(no_chars, r.no_chars);
		std::swap(mask, r.mask);
		std::swap(initial_shift, r.initial_shift);
	}

	~TabulationFunction() {
		for (size_t i = 0; i < no_chars; ++i) {
			delete[] table[i];
		}

		delete[] table;
	}

	size_t compute(size_t x, size_t m) {
		size_t y = 0;
		size_t shift = initial_shift;
		size_t mask = this->mask;
		for (size_t i = 0; i < no_chars; ++i) {
			y ^= table[i][(x & mask) >> shift];
			logger_trace << "Random pos [" << i << ", " << ((x & mask) >> shift)
					<< "] = " << table[i][((x & mask) >> shift)] << "\n";
			mask >>= char_length;
			shift -= char_length;
		}

		logger_trace << "Computing hash val of " << x << " = " << y % m
				<< ".\n";

		return y % m;
	}

};

class InsertionFailException: std::exception {

};

template<class Function>
class CuckooHashTable {
public:
	Function f, g;

	size_t max_loop;

	size_t * table;
	bool * table_presence;
	size_t table_size;
	size_t size;

	Stats stats;

	CuckooHashTable(size_t size) {
		this->size = size;
		table_size = (size_t) (size);

		table = new size_t[table_size];
		table_presence = new bool[table_size];

		for (size_t i = 0; i < table_size; ++i) {
			table_presence[i] = false;
		}

		stats.size = size;
		max_loop = 4 * (1 + log2(size));
	}

	CuckooHashTable(const CuckooHashTable<Function> & r) {
		this->size = r.size;
		table_size = r.table_size;

		this->f = r.f;
		this->g = r.g;

		table = new size_t[table_size];
		table_presence = new bool[table_size];

		for (size_t i = 0; i < table_size; ++i) {
			table_presence[i] = r.table_presence[i];
			table[i] = r.table[i];
		}

		stats = r.stats;
		max_loop = r.max_loop;
	}

	CuckooHashTable<Function> & operator=(const CuckooHashTable<Function> & r) {
		CuckooHashTable<Function> t(r);
		swap(t);
		return *this;
	}

	~CuckooHashTable() {
		logger << "D-tor cuckoo.\n";
		delete[] table;
		delete[] table_presence;
	}

	Stats & get_stats() {
		return stats;
	}

	bool insert(size_t x) {
		++stats.inserts;
		if (find(x, stats.insert_find_operations)) {
			return false;
		}

		++stats.data_size;
		return insert(x, 0);
	}

	bool find(size_t x) {
		++stats.finds;
		return find(x, stats.find_operations);
	}

	void print(std::ostream & out) {
		for (int i = 0; i < table_size; ++i) {
			if (table_presence[i]) {
				out << i << ": " << table[i] << std::endl;
			}
		}
	}

private:
	const size_t MAX_REHASH_LEVEL = 15;

	bool insert(size_t x, size_t level) {
		size_t pos = f.compute(x, table_size);
		logger_trace << "Inserting " << x << ", max-loop: " << max_loop << "\n";
		for (size_t i = 0; i < max_loop; ++i) {
			logger_trace << "Cuckoo insert pos: " << pos << ".\n";

			if (table_presence[pos] == false) {
				table_presence[pos] = true;
				table[pos] = x;
				return true;
			}

			std::swap<size_t>(x, table[pos]);
			++stats.insert_swaps;

			size_t fpos = f.compute(x, table_size);
			if (pos == fpos) {
				pos = g.compute(x, table_size);
			} else {
				pos = fpos;
			}
		}

		logger << "Cuckoo insert rehash " << x << "\n";
		rehash(level + 1);
		insert(x, level + 1);

		return true;
	}

	void rehash(size_t level) {
		if (level == MAX_REHASH_LEVEL) {
			logger << "Max rehash level reached!\n";
			throw InsertionFailException();
		}

		CuckooHashTable n(size);
		for (int i = 0; i < table_size; ++i) {
			if (table_presence[i]) {
				n.insert(table[i], level);
			}
		}

		Stats t_stats(stats);
		t_stats.add(n.stats);
		swap(n);
		stats = t_stats;
		++stats.rehashes;
	}

	void swap(CuckooHashTable<Function> & r) {
		std::swap(max_loop, r.max_loop);
		std::swap(table, r.table);
		std::swap(table_presence, r.table_presence);
		std::swap(table_size, r.table_size);
		std::swap(size, r.size);
		std::swap(stats, r.stats);
		std::swap(f, r.f);
		std::swap(g, r.g);
	}

	bool find(size_t x, size_t & stat_acc) {
		size_t y = f.compute(x, table_size);
		logger_trace << y << " " << table_size << "\n";
		++stat_acc;
		if (table_presence[y] && table[y] == x) {
			return true;
		}

		++stat_acc;
		y = g.compute(x, table_size);
		if (table_presence[y] && table[y] == x) {
			return true;
		}

		return false;
	}

};

template<class Function>
class LinearProbingHashTable {
public:
	Function f;

	size_t * table;
	bool * table_presence;
	size_t table_size;

	Stats stats;

	LinearProbingHashTable(size_t size) {
		table_size = size;

		table = new size_t[table_size];
		table_presence = new bool[table_size];

		for (size_t i = 0; i < table_size; ++i) {
			table_presence[i] = false;
		}

		stats.size = size;
	}

	~LinearProbingHashTable() {
		delete[] table;
		delete[] table_presence;
	}

	LinearProbingHashTable(const LinearProbingHashTable<Function> & r) {
		this->f = r.f;
		table_size = r.table_size;

		table = new size_t[table_size];
		table_presence = new bool[table_size];

		for (size_t i = 0; i < table_size; ++i) {
			table_presence[i] = r.table_presence[i];
			table[i] = r.table[i];
		}

		stats = r.stats;
	}

	LinearProbingHashTable<Function> & operator=(
			const LinearProbingHashTable<Function> & r) {
		LinearProbingHashTable<Function> t(r);
		swap(t);
		return *this;
	}

	Stats & get_stats() {
		return stats;
	}

	bool insert(size_t x) {
		++stats.inserts;

		size_t pos = f.compute(x, table_size);
		++stats.insert_find_operations;
		for (;; pos = (pos + 1) % table_size) {
			if (table_presence[pos] == false) {
				table_presence[pos] = true;
				table[pos] = x;
				++stats.data_size;
				return true;
			} else if (table[pos] == x) {
				return false;
			} else {
				++stats.insert_find_operations;
			}
		}

		return false;
	}

	bool find(size_t x) {
		size_t pos = f.compute(x, table_size);
		++stats.finds;
		++stats.find_operations;

		for (; ; pos = (pos + 1) % table_size) {
			if (table_presence[pos] == false) {
				return false;
			} else if (table[pos] == x) {
				return true;
			} else {
				++stats.find_operations;
			}
		}

		return false;
	}

	void print(std::ostream & out) {
		for (int i = 0; i < table_size; ++i) {
			if (table_presence[i]) {
				out << i << ": " << table[i] << std::endl;
			}
		}
	}

private:
	void swap(LinearProbingHashTable<Function> & r) {
		std::swap(table, r.table);
		std::swap(table_presence, r.table_presence);
		std::swap(table_size, r.table_size);
		std::swap(stats, r.stats);
		std::swap(f, r.f);
	}

};

template<class Function>
class TwoChoicesHashTable {
public:
	struct FirstNode;

	struct Node {

		Node(size_t x) :
				val(x), next(0) {
		}

		Node(size_t x, FirstNode * n) :
				Node(x) {
			next = n->next;
			n->next = this;
			++(n->size);
		}

		size_t val;
		Node * next;

	};

	struct FirstNode: Node {

		FirstNode(size_t x) :
				FirstNode(x, 1) {
		}

		FirstNode(size_t x, size_t _size) :
				Node(x), size(_size) {
		}

		size_t size;

	};

	Function f, g;

	FirstNode ** table;
	size_t table_size;
	size_t half_size;

	Stats stats;

	TwoChoicesHashTable(size_t size) {
		table_size = size;
		half_size = size / 2;

		table = new FirstNode *[table_size];

		for (size_t i = 0; i < table_size; ++i) {
			table[i] = 0;
		}

		stats.size = size;
	}

	~TwoChoicesHashTable() {
		for (size_t i = 0; i < table_size; ++i) {
			if (table[i]) {
				Node * t;
				for (Node * n = table[i]->next; n != 0; n = t) {
					t = n->next;
					delete n;
				}

				delete table[i];
			}
		}
		delete[] table;
	}

	TwoChoicesHashTable(const TwoChoicesHashTable<Function> & r) {
		logger_trace << "Copy c-tor.\n";

		this->f = r.f;
		this->g = r.g;

		this->table_size = r.table_size;
		this->half_size = r.half_size;

		table = new FirstNode*[table_size];

		for (size_t i = 0; i < table_size; ++i) {
			if (!r.table[i]) {
				table[i] = 0;
				continue;
			}

			table[i] = new FirstNode(r.table[i]->val, r.table[i]->size);
			for (Node * nr = r.table[i], *nt = table[i]; nr != 0; nr =
					nr->next) {
				nt->next = new Node(nr->val);
				nt = nt->next;
			}

			table[i] = r.table[i];
		}

		stats = r.stats;
	}

	TwoChoicesHashTable<Function> & operator=(
			const TwoChoicesHashTable<Function> & r) {
		TwoChoicesHashTable<Function> t(r);
		swap(t);
		return *this;
	}

	Stats & get_stats() {
		return stats;
	}

	bool insert(size_t x) {
		++stats.inserts;
		if (find(x, stats.insert_find_operations)) {
			return false;
		}

		logger_trace << "Insert " << x << "\n";

		size_t fy = f.compute(x, half_size);
		size_t gy = f.compute(x, half_size);

		if (table[fy] == 0) {
			table[fy] = new FirstNode(x);
		} else if (table[gy] == 0) {
			table[gy] = new FirstNode(x);
		} else if (table[fy]->size <= table[gy]->size) {
			new Node(x, table[fy]);
		} else {
			new Node(x, table[gy]);
		}

		++stats.data_size;
		return true;
	}

	bool find(size_t x) {
		++stats.finds;
		return find(x, stats.find_operations);
	}

	void print(std::ostream & out) {
		for (int i = 0; i < table_size; ++i) {
			if (table[i]) {
				for (Node * n = table[i]; n != 0; n = n->next) {
					out << i << ": " << n->val << std::endl;
				}
			}
		}
	}

private:
	void swap(TwoChoicesHashTable<Function> & r) {
		std::swap(table, r.table);
		std::swap(table_size, r.table_size);
		std::swap(half_size, r.half_size);
		std::swap(stats, r.stats);
		std::swap(f, r.f);
		std::swap(g, r.g);
	}

	bool search(Node * n, size_t x, size_t & stat_acc) {
		for (; n != 0; n = n->next) {
			++stat_acc;
			if (n->val == x) {
				return true;
			}
		}

		++stat_acc;
		return false;
	}

	bool find(size_t x, size_t & stat_acc) {
		size_t fy = f.compute(x, half_size);
		size_t gy = half_size + g.compute(x, half_size);
		return search(table[fy], x, stat_acc) || search(table[gy], x, stat_acc);
	}

};

template<template<class > class HashTable, class Function>
void test() {
	HashTable<Function> t(4);

	std::string operation;
	size_t inserts, table_size, operand;

	bool fail;

	for (;;) {
		std::cin >> operation;
		if (std::cin.bad() || std::cin.eof()) {
			t.get_stats().finish();
			if (fail) {
				Stats stats = t.get_stats();
				stats.data_size = inserts;
				stats.inserts = 0;
				stats.finds = 0;
				stats.running_time = 0;
				stats.printCsv(std::cout);
			} else {
				t.get_stats().printCsv(std::cout);
			}
			break;
		}

		if (operation == "#") {
			if (t.get_stats().size > 4) {
				t.get_stats().finish();

				if (fail) {
					Stats stats = t.get_stats();
					stats.data_size = inserts;
					stats.inserts = 0;
					stats.finds = 0;
					stats.running_time = 0;
					stats.printCsv(std::cout);
				} else {
					t.get_stats().printCsv(std::cout);
				}
			}

			std::cin >> inserts >> table_size;
			fail = false;
			std::cerr << "# " << inserts << " " << table_size << std::endl;
			t = HashTable<Function>(table_size);
			t.get_stats().start();
		}
		if (fail) {
			std::cin >> operand;
		} else if (operation == "I") {
			std::cin >> operand;
			try {
				t.insert(operand);
			} catch (InsertionFailException & e) {
				std::cerr << "Insertion fail!" << std::endl;
				fail = true;
			}
		} else if (operation == "F") {
			std::cin >> operand;
			t.find(operand);
		}
	}
}

int main(int argc, char ** argv) {
	init_rng();

	if (!std::strcmp("cuckoo", argv[1])) {
		if (!std::strcmp("mult", argv[2])) {
			test<CuckooHashTable, MultiplicationFunction>();
		} else if (!std::strcmp("tab", argv[2])) {
			test<CuckooHashTable, TabulationFunction>();
		}
	} else if (!std::strcmp("linear", argv[1])) {
		if (!std::strcmp("mult", argv[2])) {
			test<LinearProbingHashTable, MultiplicationFunction>();
		} else if (!std::strcmp("tab", argv[2])) {
			test<LinearProbingHashTable, TabulationFunction>();
		}
	} else if (!std::strcmp("two", argv[1])) {
		if (!std::strcmp("mult", argv[2])) {
			test<TwoChoicesHashTable, MultiplicationFunction>();
		} else if (!std::strcmp("tab", argv[2])) {
			test<TwoChoicesHashTable, TabulationFunction>();
		}
	}

	return 0;
}
