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

} }

#endif
