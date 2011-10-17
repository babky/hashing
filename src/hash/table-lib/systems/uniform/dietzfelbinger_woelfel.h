#ifndef DIETZFELBINGER_WOELFEL_H
#define DIETZFELBINGER_WOELFEL_H

#include <algorithm>
#include "systems/universal_system.h"
#include "systems/universal_system_factory.h"
#include "utils/hash_assert.h"
#include "utils/static_random_generator.h"

namespace Hash { namespace Systems { namespace Uniform {

	template<typename T, template <class> class HashFunction>
	class DietzfelbingerWoelfel : public UniversalSystem<T> {
	public:
		const static size_t START_LENGTH = 16;

		// TODO: Figure this out.
		// const static double DEFAULT_R_EXPONENT = 0.75;

		~DietzfelbingerWoelfel(void) {
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

		DietzfelbingerWoelfel(const DietzfelbingerWoelfel & a):
		  length(a.length),
		  r(a.r),
		  rExponent(a.rExponent),
		  z(new size_t[a.r]),
		  f(a.f),
		  g(a.g)
		{
			for (size_t i = 0; i < r; ++i) {
				z[i] = a.z[i];
			}
		}

		DietzfelbingerWoelfel & operator=(const DietzfelbingerWoelfel & r) {
			DietzfelbingerWoelfel tmp = r;
			tmp.swap(*this);
			return *this;
		}

		void reset(void) {
			for (size_t i = 0; i < r; ++i) {
				z[i] = Hash::Utils::StaticRandomGenerator<T>::getGenerator().generate();
			}

			f = UniversalSystemFactory<T, HashFunction>::create(getTableSize());
			g = UniversalSystemFactory<T, HashFunction>::create(r);
		}

		size_t getTableSize(void) const {
			return length;
		}
		
		void setTableSize(size_t size) {
			length = size;
			resetRandomVector();
			f.setTableSize(size);
			g.setTableSize(size);
		}

		T getUniversumMax(void) const {
			return f.getUniversumMax();
		}

		void setUniversumMax(T universumMax) {
			f.setUniversumMax(universumMax);
			g.setUniversumMax(universumMax);
		}

		size_t hash(const T & x, size_t length) {
			simple_assert(length == this->length, "Lengths must be the same.");

			return (f(x, length) + z[g(x, r)]) % length;
		}

		size_t operator()(const T & a, size_t length) {
			return hash(a, length);
		}

		void swap(DietzfelbingerWoelfel & r) {
			using std::swap;

			swap(this->length, r.length);
			swap(this->r, r.r);
			swap(this->rExponent, r.rExponent);
			swap(this->z, r.z);
			swap(this->f, r.f);
			swap(this->g, r.g);
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
