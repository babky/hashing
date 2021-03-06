#include <unordered_set>
#include <iostream>
#include "miscellaneous/linear_probing_table_implementation.h"
#include "systems/identity_function.h"
#include "systems/tr1_function.h"
#include "systems/tabulation_system.h"
#include "storages/probing_storage.h"
#include "storages/chained_storage.h"
#include "storages/direct_chaining_storage.h"
#include "utils/equality_comparer.h"
#include "table.h"
#include <boost/config.hpp>
#ifdef BOOST_MSVC
	#pragma warning(disable: 4512 4127 4100)
#endif
#include <boost/date_time.hpp>
#include <boost/integer.hpp>
#include <boost/integer_traits.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512 4127 4100)
#endif

using namespace Hash;
using namespace Hash::Systems;
using namespace Hash::Storages;
using namespace boost::posix_time;
using namespace std;

int main(int, char **) {
	typedef boost::int64_t T;
	typedef Miscellaneous::Table<T, Tr1Function<T, size_t> > LPTR1;
	typedef Table<T, Hash::Utils::EqualityComparer<T>, IdentityFunction, LinearProbingStorage> LinearProbingTabulation;
	typedef Table<T, Hash::Utils::EqualityComparer<T>, TabulationFunction, ChainedStorage> ChainingTabulation;
	typedef Table<T, Hash::Utils::EqualityComparer<T>, IdentityFunction, DirectChainingStorage> DirectChainingTabulation;

	ptime start, finish;
	size_t ELEMENT_COUNT = 1 << 24;

	DirectChainingTabulation dcht;
	start = microsec_clock::local_time();
	for (T i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		dcht.insert(i);
	}
	finish = microsec_clock::local_time();
	cout << "DCHT insertion for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;
	start = microsec_clock::local_time();
	for (T i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		dcht.contains(i);
	}
	finish = microsec_clock::local_time();
	cout << "DCHT find for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;
	dcht.clear();

	LinearProbingTabulation lpt;
	start = microsec_clock::local_time();
	for (T i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		lpt.insert(i * i);
	}
	finish = microsec_clock::local_time();
	cout << "LPT insertion for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;
	start = microsec_clock::local_time();
	for (T i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		lpt.contains(i * i);
	}
	finish = microsec_clock::local_time();
	cout << "LPT find for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;
	lpt.clear();

	LPTR1 lptr1;
	start = microsec_clock::local_time();
	for (T i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		lptr1.insert(i * i);
	}
	finish = microsec_clock::local_time();
	cout << "LPTR insertion for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;
	start = microsec_clock::local_time();
	for (T i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		lptr1.find(i * i);
	}
	finish = microsec_clock::local_time();
	cout << "LPTR find for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;
	lptr1.clear();

	start = microsec_clock::local_time();
#ifdef BOOST_MSVC
	std::tr1::unordered_set<T> mySet2;
#else
	unordered_set<T> mySet2;
#endif
	for (T i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		mySet2.insert(i * i);
	}
	finish = microsec_clock::local_time();
	cout << "US insertion for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;
	start = microsec_clock::local_time();
	for (T i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		if (mySet2.find(i * i) == mySet2.end()) {
			cout << "Not found." << endl;
		}
	}
	finish = microsec_clock::local_time();
	cout << "US find for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;
	mySet2.clear();

	return 0;
}
