#include <iostream>
#include <vector>
#include <string>
#include <boost/config.hpp>
#ifdef BOOST_MSVC
	#pragma warning(disable: 4512 4127 4100)
#endif
#include <boost/date_time.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512 4127 4100)
#endif
#include "systems/uniform/dietzfelbinger_woelfel.h"
#include "systems/bit_string_system.h"
#include "systems/polynomial_system.h"
#include "systems/linear_map_system.h"
#include "systems/cwlf_system.h"
#include "systems/tabulation_system.h"
#include "systems/polynomial_system.h"
#include "systems/tr1_function.h"

using namespace std;
using namespace boost::posix_time;

namespace Hash { namespace Systems {
	
	template<typename T, class Storage>
	class PolynomialSystem4 : public Hash::Systems::PolynomialSystem<T, Storage> {
	public:
		explicit PolynomialSystem4(size_t tableSize = StorageParams::INITIAL_STORAGE_SIZE,
			T aUniversumMax = Hash::Math::Prime<T>::GREATEST_PRIME, size_t aDegree = 4):
		  PolynomialSystem<T, Storage>(tableSize, aUniversumMax, aDegree) {
		}
	};

} }

template<typename T, class Storage>
struct SystemDescription {
	SystemDescription(Hash::Systems::UniversalFunction<T, Storage> * f, string n):
	  function(f),
	  name(n) {
	}

	Hash::Systems::UniversalFunction<T, Storage> * function;
	string name;
	time_duration time;
};

int main(void) {
	// typedef size_t T;
	typedef boost::uint64_t T;
	typedef size_t TestStorage;
	typedef SystemDescription<T, TestStorage> TestSystemDescription;
	typedef vector<TestSystemDescription> FunctionVector;
#ifdef HASH_DEBUG
	const size_t TABLE_SIZE = 1 << 10;
#else
	const size_t TABLE_SIZE = 1 << 24;
#endif

	FunctionVector functions;
	
	functions.push_back(TestSystemDescription(new Hash::Systems::UniversalFunctionLinearMap<T, TestStorage>(TABLE_SIZE), "LinearMap"));
	functions.push_back(TestSystemDescription(new Hash::Systems::UniversalFunctionCWLF<T, TestStorage>(TABLE_SIZE), "CWLF"));
	functions.push_back(TestSystemDescription(new Hash::Systems::BitStringFunction<T, TestStorage>(TABLE_SIZE), "BitString"));
	functions.push_back(TestSystemDescription(new Hash::Systems::TabulationFunction<T, TestStorage>(TABLE_SIZE), "Tabulation"));
	functions.push_back(TestSystemDescription(new Hash::Systems::Uniform::DietzfelbingerWoelfel<T, TestStorage, Hash::Systems::PolynomialSystem4>(TABLE_SIZE), "DW - deg 4"));
	functions.push_back(TestSystemDescription(new Hash::Systems::PolynomialSystem<T, TestStorage>(TABLE_SIZE), "Polynomial - deg 2"));
	functions.push_back(TestSystemDescription(new Hash::Systems::PolynomialSystem<T, TestStorage>(TABLE_SIZE, Hash::Math::Prime<T>::GREATEST_PRIME, 32), "Polynomial - deg 32"));
	functions.push_back(TestSystemDescription(new Hash::Systems::Tr1Function<T, TestStorage>(TABLE_SIZE), "TR1"));
	
	for (FunctionVector::iterator b = functions.begin(), e = functions.end(); b != e; ++b) {
		b->function->setTableSize(TABLE_SIZE);
		b->function->reset();

		ptime start = microsec_clock::local_time();
		for (size_t i = 0; i < TABLE_SIZE / 2; ++i) {
			b->function->hash(i);
		}
		ptime finish = microsec_clock::local_time();
		b->time = finish - start;
		
		cout << b->name << ": " << b->time.total_milliseconds() << " ms" << endl;
	}
	
	return 0;
}
