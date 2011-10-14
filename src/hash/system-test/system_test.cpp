#include <iostream>
#include <vector>
#include <string>
#include <boost/date_time.hpp>
#include "systems/uniform/dietzfelbinger_woelfel.h"
#include "systems/polynomial_system.h"
#include "systems/linear_map_system.h"
#include "systems/cwlf_system.h"
#include "systems/polynomial_system.h"

using namespace std;
using namespace boost::posix_time;

namespace Hash { namespace Systems {
	
	template<typename T>
	class PolynomialSystem4 : public Hash::Systems::PolynomialSystem<T> {
	public:
		explicit PolynomialSystem4(size_t aStartLength = Hash::Systems::PolynomialSystem<T>::START_LENGTH, T aUniversumMax = Hash::Math::Prime<T>::GREATEST_PRIME, size_t aDegree = 4):
		  PolynomialSystem<T>(aStartLength, aUniversumMax, aDegree) {
		}
	};

} }

template<typename T>
struct SystemDescription {
	SystemDescription(Hash::Systems::UniversalSystem<T> * f, string n):
	  function(f),
	  name(n)
	{
	}

	Hash::Systems::UniversalSystem<T> * function;
	string name;
	time_duration time;
};

int main(void) {
	// typedef size_t T;
	typedef boost::uint64_t T;
	typedef vector<SystemDescription<T> > FunctionVector;
	const size_t TABLE_LENGTH = 1 << 20;

	FunctionVector functions;
	
	functions.push_back(SystemDescription<T>(new Hash::Systems::UniversalFunctionLinearMap<T>(TABLE_LENGTH), "LinearMap"));
	functions.push_back(SystemDescription<T>(new Hash::Systems::UniversalFunctionCWLF<T>(TABLE_LENGTH), "CWLF"));
	functions.push_back(SystemDescription<T>(new Hash::Systems::PolynomialSystem<T>(TABLE_LENGTH), "Polynomial - deg 2"));
	functions.push_back(SystemDescription<T>(new Hash::Systems::Uniform::DietzfelbingerWoelfel<T, Hash::Systems::PolynomialSystem4>(TABLE_LENGTH), "DW - deg 4"));
	functions.push_back(SystemDescription<T>(new Hash::Systems::PolynomialSystem<T>(TABLE_LENGTH, Hash::Math::Prime<T>::GREATEST_PRIME, 32), "Polynomial - deg 32"));
	
	for (FunctionVector::iterator b = functions.begin(), e = functions.end(); b != e; ++b) {

		ptime start = microsec_clock::local_time();
		for (size_t i = 0; i < 1 << 23; ++i) {
			b->function->hash(i, TABLE_LENGTH);
		}
		ptime finish = microsec_clock::local_time();
		b->time = finish - start;
		
		cout << b->name << ": " << b->time.total_milliseconds() << " ms" << endl;
	}
	
	return 0;
}