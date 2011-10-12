#ifndef DIETZFELBINGER_WOELFEL_H
#define DIETZFELBINGER_WOELFEL_H

#include "systems/universal_system.h"
#include "systems/universal_system_factory.h"
#include "utils/hash_assert.h"
#include "utils/static_random_generator.h"

namespace Hash { namespace Systems { namespace Uniform {

	template<typename T, template <class> class HashFunction>
	class DietzfelbingerWoelfel : public UniversalSystem<T> {
	public:
		const static size_t START_LENGTH = 16;
		// const static double DEFAULT_R_EXPONENT = 0.75;

		virtual ~DietzfelbingerWoelfel(void) {
			delete [] z;
		}

		DietzfelbingerWoelfel(size_t aLength = START_LENGTH, double arExponent = 0.75):
		  length(aLength),
		  rExponent(arExponent),
		  z(0)
		{
			resetRandomVector();
			reset();
		}

		virtual void reset(void) {
			for (size_t i = 0; i < r; ++i) {
				z[i] = Hash::Utils::StaticRandomGenerator<T>::getGenerator().generate();
			}

			f = UniversalSystemFactory<T, HashFunction>::create(getTableSize());
			g = UniversalSystemFactory<T, HashFunction>::create(r);
		}

		virtual size_t getTableSize(void) const {
			return length;
		}
		
		virtual void setTableSize(size_t size) {
			length = size;
			resetRandomVector();
			f.setTableSize(size);
			g.setTableSize(size);
		}

		virtual T getUniversumMax(void) const {
			return f.getUniversumMax();
		}

		virtual void setUniversumMax(T universumMax) {
			f.setUniversumMax(universumMax);
			g.setUniversumMax(universumMax);
		}

		virtual size_t hash(const T & x, size_t length) {
			simple_assert(length == this->length, "Lengths must be the same.");

			return (f(x, length) + z[g(x, r)]) % length;
		}

		virtual size_t operator()(const T & a, size_t length) {
			return hash(a, length);
		}

	protected:
		virtual void resetRandomVector(void) {
			r = static_cast<size_t> (ceil(pow(length, rExponent)));
			delete [] z;
			z = new size_t[r];
		}

		size_t length, r;
		double rExponent;
		size_t * z;
		HashFunction<T> f, g;
	};

} } }

#endif /* DIETZFELBINGER_WOELFEL_H */
