#include "static_random_generator.h"

using namespace Hash::Utils;

template<> RandomGenerator<size_t> * StaticRandomGenerator<size_t>::generator = 0;
