#include <iostream>
#include "systems/uniform/dietzfelbinger_woelfel.h"
#include "systems/polynomial_system.h"

using namespace std;

int main(void) {
	const size_t TABLE_SIZE = 64;
	Hash::Systems::Uniform::DietzfelbingerWoelfel<size_t, Hash::Systems::PolynomialSystem> h(TABLE_SIZE);
	
	for (size_t i = 0; i < TABLE_SIZE; ++i) {
		std::cout << h(i, TABLE_SIZE) << std::endl;
	}
	
	return 0;
}