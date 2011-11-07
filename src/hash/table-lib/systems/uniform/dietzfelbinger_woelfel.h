#ifndef DIETZFELBINGER_WOELFEL_H
#define DIETZFELBINGER_WOELFEL_H

#include <algorithm>
#include "systems/universal_system.h"
#include "systems/universal_system_factory.h"
#include "utils/hash_assert.h"
#include "utils/static_random_generator.h"
#include "storage.h"

namespace Hash { namespace Systems { namespace Uniform {

	/**
	 * The universal system designed by Dietzfelbinger and Woelfel which is almost uniform.
	 */
	template<typename T, class Storage, template <class, class> class HashFunction>
	class DietzfelbingerWoelfel : public UniversalFunction<T, Storage> {
	public:
		// TODO: Figure this out.
		// const static double DEFAULT_R_EXPONENT = 0.75;

		~DietzfelbingerWoelfel(void) {
			delete [] z;
		}

		DietzfelbingerWoelfel(size_t aTableSize = StorageParams::INITIAL_STORAGE_SIZE, double arExponent = 0.75):
		  tableSize(aTableSize),
		  rExponent(arExponent),
		  z(0)
		{
			resetRandomVector();
			reset();
		}

		DietzfelbingerWoelfel(const DietzfelbingerWoelfel & a):
		  tableSize(a.tableSize),
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
				z[i] = Hash::Utils::StaticRandomGenerator<size_t>::getGenerator().generate();
			}

			f = UniversalFunctionFactory<T, Storage, HashFunction>::create(getTableSize());
			g = UniversalFunctionFactory<T, Storage, HashFunction>::create(r);
		}

		size_t getTableSize(void) const {
			return tableSize;
		}

		void setStorage(Storage * aStorage) {
			f.setStorage(aStorage);
			g.setStorage(aStorage);
		}
		
		void setTableSize(size_t aTableSize) {
			tableSize = aTableSize;
			resetRandomVector();
			f.setTableSize(aTableSize);
			g.setTableSize(aTableSize);
		}

		T getUniversumMax(void) const {
			return f.getUniversumMax();
		}

		void setUniversumMax(T universumMax) {
			f.setUniversumMax(universumMax);
			g.setUniversumMax(universumMax);
		}

		size_t hash(const T & x) {
			return (f(x) + z[g(x)]) % tableSize;
		}

		size_t operator()(const T & a) {
			return hash(a);
		}

		void swap(DietzfelbingerWoelfel & r) {
			using std::swap;

			swap(tableSize, r.tableSize);
			swap(r, r.r);
			swap(rExponent, r.rExponent);
			swap(z, r.z);
			swap(f, r.f);
			swap(g, r.g);
		}

	protected:
		/**
		 * Resets the random vector. This method does the possible resizing and initializes the vector to new random 
		 * values.
		 */
		virtual void resetRandomVector(void) {
			r = static_cast<size_t> (ceil(pow(tableSize, rExponent)));
			delete [] z;
			z = new size_t[r];
		}

		/**
		 * Length of the table.
		 */
		size_t tableSize;

		/**
		 * Length of the z array.
		 */
		size_t r;

		/**
		 * Exponent - size of the z array w.r.t length.
		 */
		double rExponent;

		/**
		 * Random vector of length r.
		 */
		size_t * z;

		/**
		 * The required universal hash function.
		 */
		HashFunction<T, Storage> f, g;
	};

} } }

#endif /* DIETZFELBINGER_WOELFEL_H */
