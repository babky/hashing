#include "storages/collision_count_storage.h"
#include "systems/cwlf_system.h"
#include "systems/random_system.h"
#include "table.h"
#include "utils/equality_comparer.h"
#include "utils/storage_statistics.h"
#include <iostream>
#include <boost/format.hpp>

/**
 * Checks the induction assumption for when computing the fact that.
 *
 * Let G be a positive integer and g, h \in [G]. We prove that [m] \cap h +_G  g G [G/m] is a
 * union of at most two arithmetic progressions having the same difference.
 */

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

template<typename base_t>
bool verifyAP(std::set<base_t> intersection) {
	typedef std::set<base_t> base_set;

	bool isArithmeticProgression = true;
	base_t lastElement = 0;
	base_t diff = 0;
	size_t i = 0;
	for (typename base_set::iterator it = intersection.begin(), end = intersection.end(); it != end; ++it) {
		if (i == 0) {
			lastElement = *it;
		} else if (i == 1) {
			diff = (*it) - lastElement;
		} else if (((*it) - lastElement) != diff) {
			isArithmeticProgression = false;
		}

		++i;
		lastElement = *it;
	}

	return isArithmeticProgression;
}

template<typename base_t>
bool verifyTwoAPs(std::set<base_t> intersection) {
	typedef std::set<base_t> base_set;
	typedef std::map<base_t, size_t> difference_map;

	base_t lastElement = 0;
	difference_map difference_counts;
	size_t i = 0;
	for (typename base_set::iterator it = intersection.begin(), end = intersection.end(); it != end; ++it) {
		if (i == 0) {
			lastElement = *it;
		} else {
			base_t diff = (*it) - lastElement;
			if (difference_counts.find(diff) != difference_counts.end()) {
				++difference_counts.at(diff);
			} else {
				difference_counts.insert(std::pair<base_t, size_t>(diff, 1));
			}
		}

		++i;
		lastElement = *it;
	}

	if (difference_counts.size() > 2) {
		return false;
	}

	typename difference_map::iterator it = difference_counts.begin();
	if (it->second > 1 && (++it)->second > 1) {
		return false;
	}

	return true;
}

void performTest() {
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
	//  1. [m] \cap h + g[G/m] is union of two APs.
	//  2. if G > m, then [m] \cap h + g[G/m] comes from the first elements of the leaps.

	// Generate a random g, G, h.

	base_t p = 101483;
	Hash::Utils::IntegralGeneratorWrapper<base_t> gen = Hash::Utils::IntegralGeneratorWrapper<base_t>(0, p);

	for (base_t r = 0; r < p * p; ++r) {
		base_t G = 0;
		while (G == 0) {
			G = gen.generate();
		}
		base_t g = gen.generate() % G;
		base_t m = 0;
		while (m == 0) {
			m = gen.generate();
		}
		base_t h = gen.generate();

		// The list of elements coming from the APs.
		set<base_t> intersection1, intersection2;
		bool leapt = false;

		bool comesFromFirst = true;
		for (base_t i = 0; i < G / m; ++i) {
			base_t prev = Hash::Math::UnsignedDoubleWord<base_t>::linear(g, i - 1, h, G);
			base_t res = Hash::Math::UnsignedDoubleWord<base_t>::linear(g, i, h, G);
			bool leap = (res < prev);
			leapt |= leap;

			if (res < m) {
				if (leapt) {
					if (intersection1.find(res) == intersection1.end()) {
						intersection1.insert(res);
					}
				} else {
					if (intersection2.find(res) == intersection2.end()) {
						intersection2.insert(res);
					}
				}
			}

			if (i == 0 || g < m) {
				continue;
			}

			// Verification of Assertion 2.
			if (!leap && res < m) {
				comesFromFirst = false;
			}
		}

		// Verification of Lemma.
		set<base_t> intersection;
		if (!verifyTwoAPs(intersection)) {
			cout << "Lemma does not hold! g: " << g << ", G: " << G << ", m: " << m << ", h: " << h << endl;
			return;
		}

		// Verification of Induction base case for g < m.
		if (g < m) {
			if (!verifyAP(intersection1)) {
				cout << "Assertion 1 failed for intersection 1, g: " << g << ", G: " << G << ", m: " << m << ", h: " << h << endl;
			}

			if (!verifyAP(intersection2)) {
				cout << "Assertion 1 failed for intersection 2, g: " << g << ", G: " << G << ", m: " << m << ", h: " << h << endl;
			}
		}

		if (!comesFromFirst) {
			cout << "Assertion 2 failed for g: " << g << ", G: " << G << ", m: " << m << ", h: " << h << endl;
		}
	}
}

int main(int argc, const char ** argv) {
	performTest();
	return 0;
}
