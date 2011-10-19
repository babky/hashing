#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <string>
#include <boost/config.hpp>
#ifdef BOOST_MSVC
	#pragma warning(disable: 4512 4127 4100)
#endif
#include <boost/date_time.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512 4127 4100)
#endif
#include "table.h"
#include "function.h"
#include "utils/equality_comparer.h"
#include "universal_systems.h"
#include "storage.h"
#include "policies/guaranteed_rehash_policy.h"
#include "storages/chained_storage.h"
#include "storages/bounded_chained_storage.h"
#include "systems/linear_map_system.h"
#include "systems/two_way_system.h"
#include "utils/equality_comparer.h"
#include "utils/storage_statistics.h"
#include "systems/uniform/dietzfelbinger_woelfel.h"
#include "systems/bit_string_system.h"
#include "systems/polynomial_system.h"
#include "systems/linear_map_system.h"
#include "systems/cwlf_system.h"
#include "systems/tabulation_system.h"
#include "systems/polynomial_system.h"
#include "storages/probing_storage.h"
#include "systems/two_way_system_randomized.h"
#include "math/double_word.h"

using namespace std;
using namespace boost::posix_time;
using namespace Hash;
using namespace Hash::Utils;
using namespace Hash::Systems;
using namespace Hash::Systems::Uniform;
using namespace Hash::Storages;
using namespace Hash::Policies::Rehash;

namespace Hash { namespace Systems {
	
	template<typename T, class Storage>
	class PolynomialSystem5 : public Hash::Systems::PolynomialSystem<T, Storage> {
	public:
		explicit PolynomialSystem5(size_t aStartLength = StorageParams::INITIAL_STORAGE_SIZE, T aUniversumMax = Hash::Math::Prime<T>::GREATEST_PRIME, size_t aDegree = 5):
		  PolynomialSystem(aStartLength, aUniversumMax, aDegree) {
		}
	};

	template<typename T, class Storage>
	class PolynomialSystem32 : public Hash::Systems::PolynomialSystem<T, Storage> {
	public:
		explicit PolynomialSystem32(size_t aStartLength = StorageParams::INITIAL_STORAGE_SIZE, T aUniversumMax = Hash::Math::Prime<T>::GREATEST_PRIME, size_t aDegree = 32):
		  PolynomialSystem(aStartLength, aUniversumMax, aDegree) {
		}
	};

} }

template<typename T>
class HashTableWrapper {
public:
	virtual ~HashTableWrapper(void) {
	}

	virtual const string & getName(void) const = 0;
	virtual time_duration runTest(size_t length, double maxAlpha) = 0;
	virtual void setMaxLoadFactor(double alpha) = 0;
};


template<typename T, class Table>
class HashTableWrapperImpl : public HashTableWrapper<T> {
public:
	HashTableWrapperImpl(string aName):
	  name(aName)
	{
	}

	virtual const string & getName(void) const {
		return name;
	}

	virtual time_duration runTest(size_t length, double maxAlpha) {
		ptime start, finish;
		setMaxLoadFactor(maxAlpha);
		clear();

		start = microsec_clock::local_time();
		for (size_t i = 0, e = length; i < e; ++i) {
			t.insert(i);
		}

		for (size_t i = 0, e = length; i < e; ++i) {
#ifdef HASH_DEBUG
			if (!t.contains(i)) {
				cout << "Should contain " << i << "." << endl;
			}
#else
			t.contains(i);
#endif
		}

#ifdef HASH_DEBUG
		if (t.contains(length + 1)) {
			cout << "Should not contain " << length + 1 << "." << endl;
		}
#endif
		finish = microsec_clock::local_time();
		clear();
		return finish - start;
	}

	void clear(void) {
		t.clear();
	}

	void setMaxLoadFactor(double alpha) {
		t.setRehashPolicy(LoadFactorBoundsRehashPolicy(LoadFactorBoundsRehashPolicy::DEFAULT_MIN_LOAD_FACTOR, alpha));
	}

private:
	Table t;
	string name;
};

typedef size_t T;
// typedef boost::uint64_t T;

typedef Table<T, Hash::Utils::EqualityComparer<T>, UniversalFunctionCWLF, ChainedStorage> ChainingLinear;
typedef Table<T, Hash::Utils::EqualityComparer<T>, TabulationFunction, ChainedStorage> ChainingTabulation;
typedef Table<T, Hash::Utils::EqualityComparer<T>, BitStringFunction, ChainedStorage> ChainingBitString;
typedef Table<T, Hash::Utils::EqualityComparer<T>, PolynomialSystem, ChainedStorage> ChainingPolynomial;
typedef Table<T, Hash::Utils::EqualityComparer<T>, PolynomialSystem5, ChainedStorage> ChainingPolynomial5;
typedef Table<T, Hash::Utils::EqualityComparer<T>, PolynomialSystem32, ChainedStorage> ChainingPolynomial32;
typedef Table<T, Hash::Utils::EqualityComparer<T>, UniversalFunctionLinearMap, ChainedStorage> ChainingLinearMap;
typedef Table<T, Hash::Utils::EqualityComparer<T>, UniversalFunctionCWLF, LinearProbingStorage> LinearProbingLinear;
typedef Table<T, Hash::Utils::EqualityComparer<T>, TabulationFunction, LinearProbingStorage> LinearProbingTabulation;
typedef Table<T, Hash::Utils::EqualityComparer<T>, PolynomialSystem5, LinearProbingStorage> LinearProbingPolynomial5;
typedef Table<T, Hash::Utils::EqualityComparer<T>, UniversalFunctionLinearMap, LinearProbingStorage> LinearProbingLinearMap;

int main(void) {
	typedef vector<double> LoadFactorVector;
	typedef vector<size_t> SizeVector;
	typedef vector<HashTableWrapper<T> * > TableVector;

	LoadFactorVector loadFactors;
	loadFactors.push_back(0.5);
	loadFactors.push_back(0.75);
	loadFactors.push_back(0.9);

	SizeVector sizes;
	sizes.push_back(1 << 10);
	sizes.push_back(1 << 16);
	sizes.push_back(1 << 20);
	sizes.push_back(1 << 24);
	sizes.push_back(1 << 27);

	TableVector tables;
	tables.push_back(new HashTableWrapperImpl<T, ChainingLinear>("ChainingLinear"));
	tables.push_back(new HashTableWrapperImpl<T, ChainingTabulation>("ChainingTabulation"));
	tables.push_back(new HashTableWrapperImpl<T, ChainingBitString>("ChainingBitString"));
	tables.push_back(new HashTableWrapperImpl<T, ChainingPolynomial>("ChainingPolynomial"));
	tables.push_back(new HashTableWrapperImpl<T, ChainingLinearMap>("ChainingLinearMap"));
	// tables.push_back(new HashTableWrapperImpl<T, ChainingPolynomial5>("ChainingPolynomial5"));
	// tables.push_back(new HashTableWrapperImpl<T, ChainingPolynomial32>("ChainingPolynomial32"));
	tables.push_back(new HashTableWrapperImpl<T, LinearProbingLinear>("LinearProbingLinear"));
	tables.push_back(new HashTableWrapperImpl<T, LinearProbingTabulation>("LinearProbingTabulation"));
	// tables.push_back(new HashTableWrapperImpl<T, LinearProbingPolynomial5>("LinearProbingPolynomial5"));
	tables.push_back(new HashTableWrapperImpl<T, LinearProbingLinearMap>("LinearProbingLinearMap"));
	
	time_duration testTime;
	for (LoadFactorVector::iterator bLF = loadFactors.begin(), eLF = loadFactors.end(); bLF != eLF; ++bLF) {
		for (SizeVector::iterator bS = sizes.begin(), eS = sizes.end(); bS != eS; ++bS) {
			for (TableVector::iterator bT = tables.begin(), eT = tables.end(); bT != eT; ++bT) {
				testTime = (*bT)->runTest(*bS, *bLF);
				cout << (*bT)->getName() << " for " << *bS << " elements and alpha=" << *bLF;
				cout << " took " << testTime.total_milliseconds() << " ms." << endl;
			}
		}
	}

	size_t ELEMENT_COUNT = 1 << 20;
	ptime start, finish;

	start = microsec_clock::local_time();
	set<T> mySet;
	for (size_t i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		mySet.insert(i);
	}
	for (size_t i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		mySet.find(i);
	}
	finish = microsec_clock::local_time();
	cout << "RBT for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;
	mySet.clear();

	start = microsec_clock::local_time();
#ifdef BOOST_MSVC
	std::tr1::unordered_set<T> mySet2;
#else
	unordered_set<T> mySet2;
#endif
	for (size_t i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		mySet2.insert(i * i * 64);
	}
	for (size_t i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		mySet2.find(i * i * 64);
	}
	finish = microsec_clock::local_time();
	cout << "US for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;
	mySet2.clear();

	start = microsec_clock::local_time();
	LinearProbingTabulation lpt;
	for (size_t i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		lpt.insert(i);
	}
	for (size_t i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		lpt.contains(i);
	}
	finish = microsec_clock::local_time();
	cout << "LPT for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;

	return 0;
}
