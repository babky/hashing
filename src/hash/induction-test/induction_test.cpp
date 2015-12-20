#include "storages/collision_count_storage.h"
#include "systems/cwlf_system.h"
#include "systems/random_system.h"
#include "table.h"
#include "utils/equality_comparer.h"
#include "utils/storage_statistics.h"
#include <iostream>
#include <boost/format.hpp>

#ifdef BOOST_MSVC
	#pragma warning(disable: 4512)
#endif
#include <boost/program_options.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512)
#endif

template <typename base_t>
base_t compute_s_m_i(base_t s, size_t m, size_t i) {
	return Hash::Math::UnsignedDoubleWord<base_t>::add(
		i,
		Hash::Math::UnsignedDoubleWord<base_t>::multiply(s, m, Hash::Math::Prime<base_t>::GREATEST_PRIME),
		Hash::Math::Prime<base_t>::GREATEST_PRIME
	);
}

template <typename base_t, typename Table>
/**
 * @param f Multiplactive factor - number of children per one progression == length of the progression.
 * @param s Offset.
 */
void generate_data(Table & t, size_t cell_height, size_t n, size_t m, size_t f, size_t cell_count, base_t s) {
	if (cell_height == 0) {
		for (size_t i = 0; i < n / cell_count; ++i) {
			// std::cout << compute_s_m_i(s, m, i) << " ";
			t.insert(compute_s_m_i<base_t>(s, m, i));
		}
	} else {
		for (size_t i = 0; i < f;  ++i) {
			generate_data(t, cell_height - 1, n, m, f, cell_count * f, compute_s_m_i<base_t>(s, m, i));
		}
	}
}

template <typename base_t>
class verify_set {
public:
	void insert(base_t x) {
		if (s.find(x) != s.end()) {
			throw std::logic_error((boost::format("Duplicate key %1") % x).str());
		}

		s.insert(x);
	}

	void print() const {
		for (typename set_type::const_iterator it = s.begin(); it != s.end(); ++it) {
			std::cout << *(it) << "\n";
		}
	}

private:
	typedef std::set<base_t> set_type;
	set_type s;
};

size_t compute_hierarchy_factor(size_t n) {
	return exp2((size_t) (log2(n) / log2(log2(n))));
}

template <typename base_t, typename Table>
void performTest(size_t runs, size_t n, size_t m, size_t cell_height, bool print_set) {
	using namespace Hash;
	using namespace Hash::Storages;
	using namespace Hash::Systems;
	using namespace Hash::Utils;

	using namespace std;

	size_t max = 0, min = m, sum = 0;

	Table t;
	t.reserve(m);

	verify_set<base_t> s;
	generate_data<base_t, verify_set<base_t>>(s, cell_height, n, m, compute_hierarchy_factor(n), 1, 0);
	if (print_set) {
		s.print();
	}

	// Run it.
	for (size_t run = 0; run < runs; ++run) {
		t.clear();
		generate_data<base_t, Table>(t, cell_height, n, m, compute_hierarchy_factor(n), 1, 0);

		StorageStatistics stats;
		t.computeStatistics(stats);
		cout << stats << endl;

		if (max < stats.getMaxChainLength()) {
			max = stats.getMaxChainLength();
		}

		if (min > stats.getMaxChainLength()) {
			min = stats.getMaxChainLength();
		}

		sum += stats.getMaxChainLength();
	}

	cout << "MIN:\t" << min << "\n";
	cout << "AVG:\t" << ((double) sum) / runs << "\n";
	cout << "MAX:\t" << max << endl;
}

int main(int argc, const char ** argv) {
	// Definitions.
	using namespace Hash;
	using namespace Hash::Storages;
	using namespace Hash::Systems;
	using namespace Hash::Utils;

	using namespace std;

	using namespace boost;
	using namespace boost::program_options;

#ifdef __GNUC__
	#ifdef __LP64__
		// typedef __uint128_t base_t;
		typedef uint64_t base_t;
	#else
		typedef uint64_t base_t;
	#endif
#else
	typedef uint64_t base_t;
#endif

	// Verify the induction now.

	// For each G in [p] we have:
	//  1. [m] \cap g[G/m] an AP.
	//  2. if G > m, then [m] \cap g[G/m] comes from the first elements of the leaps.

	// Generate a random g, G.

	base_t p = 101483;
	Hash::Utils::IntegralGeneratorWrapper<base_t> gen = Hash::Utils::IntegralGeneratorWrapper<base_t>(0, p);

	for (base_t r = 0; r < p * p; ++r) {
		base_t g = gen.generate();
		base_t G = gen.generate();
		base_t m = 0;
		while (m == 0) {
			m = gen.generate();
		}

		set<base_t> intersection;

		bool comesFromFirst = true;
		for (base_t i = 0; i < G/m; ++i) {
			base_t res = Hash::Math::UnsignedDoubleWord<base_t>::linear(g, i, 0, p);
			if (res < m && intersection.find(res) == intersection.end()) {
				intersection.insert(res);
			}

			if (i == 0 || g < m) {
				continue;
			}

			// Verification of Assertion 2.
			bool leap = res < Hash::Math::UnsignedDoubleWord<base_t>::linear(g, i - 1, 0, p);
			if (!leap && res < m) {
				comesFromFirst = false;
			}
		}

		// Verification of Assertion 1.
		bool isArithmeticProgression = true;
		base_t lastElement = 0;
		base_t diff = 0;
		for (set<base_t>::iterator it = intersection.begin(), end = intersection.end(); it != end; ++it) {
			if ((*it) == 0) {
				continue;
			}

			if (lastElement == 0) {
				diff = *it;
			} else if (((*it) - lastElement) != diff) {
				isArithmeticProgression = false;
			}

			lastElement = *it;
		}

		if (!isArithmeticProgression) {
			cout << "Assertion 1 failed for g: " << g << ", G: " << G << ", m: " << m << endl;
		}

		if (!comesFromFirst) {
			cout << "Assertion 2 failed for g: " << g << ", G: " << G << ", m: " << m << endl;
		}
	}

	return 0;
}
