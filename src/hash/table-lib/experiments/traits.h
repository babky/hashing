#ifndef TRAITS_H
#define TRAITS_H

#include "../systems/multiply_shift_system.h"
#include "../systems/bad_linear_system.h"

namespace Hash { namespace Experiments {

template<class HashFunction>
struct GeneratorFactoryTraits {

	typedef typename HashFunction::Generator Generator;

	static Generator create_generator(size_t universeSize, size_t tableSize) {
		return Generator(universeSize, tableSize);
	}

};

template<typename T, class Storage>
struct GeneratorFactoryTraits<Hash::Systems::MultiplyShiftSystem<T, Storage>> {

	typedef typename Hash::Systems::MultiplyShiftSystem<T, Storage>::Generator Generator;

	static Generator create_generator(size_t universeSize, size_t tableSize) {
		return Generator(universeSize - 1, tableSize);
	}

};

template<typename T, class Storage>
struct GeneratorFactoryTraits<Hash::Systems::BadLinearSystem<T, Storage>> {

	typedef typename Hash::Systems::BadLinearSystem<T, Storage>::Generator Generator;

	static Generator create_generator(size_t universeSize, size_t tableSize) {
		return Generator(tableSize);
	}

};

template<class System>
class RandomFunctionGenerator {
public:
	RandomFunctionGenerator(size_t aSampleCount, size_t universeSize, size_t tableSize):
	  sampleCount(aSampleCount) { 
		f.setUniversumMax(universeSize - 1);
		f.setTableSize(tableSize);
	}

	bool hasNext(void) const {
		return sampleCount != 0;
	}

	System next() {
		f.reset();
		--sampleCount;
		return f;
	}

private:
	System f;
	size_t sampleCount;
};

template<class System>
struct RandomFunctionGeneratorFactoryTraits {

	using Generator = RandomFunctionGenerator<System>;

	static RandomFunctionGenerator<System> create_generator(size_t universeSize, size_t tableSize) {
		return RandomFunctionGenerator<System>(1024, universeSize, tableSize);
	}

};


} }

#endif
