#include <unordered_set>
#include <iostream>
#include "miscellaneous/linear_probing_table_implementation.h"
#include "systems/tr1_function.h"
#include <boost/config.hpp>
#ifdef BOOST_MSVC
	#pragma warning(disable: 4512 4127 4100)
#endif
#include <boost/date_time.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512 4127 4100)
#endif

using namespace Hash;
using namespace boost::posix_time;
using namespace std;

int main(int, char **) {
	typedef size_t T;
	typedef Miscellaneous::Table<T, Hash::Systems::Tr1Function<T, size_t> > LPT;

	ptime start, finish;

	LPT lpt;
	size_t ELEMENT_COUNT = 1 << 24;
	start = microsec_clock::local_time();
	for (size_t i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		lpt.insert(i);
	}
	finish = microsec_clock::local_time();
	cout << "LPT insertion for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;
	start = microsec_clock::local_time();
	for (size_t i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		lpt.find(i);
	}
	finish = microsec_clock::local_time();
	cout << "LPT find for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;

	start = microsec_clock::local_time();
#ifdef BOOST_MSVC
	std::tr1::unordered_set<T> mySet2;
#else
	unordered_set<T> mySet2;
#endif
	for (size_t i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		mySet2.insert(i);
	}
	finish = microsec_clock::local_time();
	cout << "US insertion for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;
	start = microsec_clock::local_time();
	for (size_t i = 0, e = ELEMENT_COUNT; i < e; ++i) {
		mySet2.find(i);
	}
	finish = microsec_clock::local_time();
	cout << "US find for " << ELEMENT_COUNT << " elements took "<< (finish - start).total_milliseconds() << " ms." << endl;
	mySet2.clear();


	return 0;
}