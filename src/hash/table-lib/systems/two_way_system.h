#ifndef TWO_WAY_SYSTEM_H
#define TWO_WAY_SYSTEM_H

#include "systems/linear_map_system.h"
#include "systems/polynomial_system.h"
#include "systems/cwlf_system.h"
#include "systems/cwlf_exponential_system.h"
#include "systems/universal_system.h"
#include "utils/hash_assert.h"
#include <algorithm>

namespace Hash { namespace Systems {

	/**
	 * System allowing two way universal hash function. The value is hashed to the shorter chain.
	 *
	 * @typeparam T Type of the hashed values.
	 * @typeparam Storage Storage used.
	 * @typeparam System The universal system which will be used.
	 */
	template <typename T, class Storage, template <typename, class> class System>
	class TwoWaySystem : public UniversalFunction<T, Storage> {
	public:
		// Use the former HashType;
		using UniversalFunction<T, Storage>::HashType;

		/**
		 * Single function type, it is chosen randomly from the given universal system.
		 */
		typedef System<T, Storage> Function;

		/**
		 * C-tor.
		 */
		TwoWaySystem(void):
		  storage(0) {
			reset();
		}

		virtual ~TwoWaySystem(void) {
		}

		virtual void reset(void) {
			f.reset();
			g.reset();
		}

		virtual size_t getTableSize(void) const {
			simple_assert(f.getTableSize() == g.getTableSize(), "Lengths for the two functions must be the same.");
			return f.getTableSize();
		}

		virtual void setTableSize(size_t length) {
			f.setTableSize(length);
			g.setTableSize(length);
		}

		virtual void setStorage(Storage * aStorage) {
			storage = aStorage;

			f.setStorage(aStorage);
			g.setStorage(aStorage);
		}

		virtual T getUniversumMax(void) const {
			simple_assert(f.getUniversumMax() == g.getUniversumMax(), "Universum sizes the two functions must be the same.");
			return f.getUniversumMax();
		}

		virtual void setUniversumMax(T universumMax) {
			f.setUniversumMax(universumMax);
			g.setUniversumMax(universumMax);
		}

		virtual size_t hash(const T & x) {
			size_t hf = f.hash(x);
			size_t hg = g.hash(x);

			return storage->getChainLength(hf) < storage->getChainLength(hg) ? hf : hg;
		}

		virtual size_t operator()(const T & a) {
			return hash(a);
		}

		void swap(TwoWaySystem & b) {
			using std::swap;

			swap<Function>(f, b.f);
			swap<Function>(g, b.g);
		}

		friend void swap(TwoWaySystem & a, TwoWaySystem & b) {
			a.swap(b);
		}

	private:
		/**
		 * Universal function.
		 */
		Function f;

		/**
		 * Universal function.
		 */
		Function g;

		/**
		 * Storage info.
		 */
		Storage * storage;
	};

	/**
	 * The possibility of two way linear hashing.
	 */
	template <typename T, class Storage>
	class TwoWaySystemLinearMap : public Hash::Systems::TwoWaySystem<T, Storage, Hash::Systems::UniversalFunctionLinearMap> {
	};

	/**
	 * The possibility of two way polynomial hashing.
	 */
	template <typename T, class Storage>
	class TwoWaySystemPolynomial : public Hash::Systems::TwoWaySystem<T, Storage, Hash::Systems::PolynomialSystem> {
	};

	/**
	 * The possibility of two way CWLF.
	 */
	template <typename T, class Storage>
	class TwoWaySystemCWLF : public Hash::Systems::TwoWaySystem<T, Storage, Hash::Systems::UniversalFunctionCWLF> {
	};

	/**
	 * The possibility of two way CWLFExponential.
	 */
	template <typename T, class Storage>
	class TwoWaySystemCWLFExponential : public Hash::Systems::TwoWaySystem<T, Storage, Hash::Systems::CWLFExponentialSystem> {
	};


} }

namespace std {

	template <typename T, class Storage, template <typename, class> class System>
	void swap(Hash::Systems::TwoWaySystem<T, Storage, System> & a, Hash::Systems::TwoWaySystem<T, Storage, System> & b) {
		a.swap(b);
	}

	template <typename T, class Storage>
	void swap(Hash::Systems::TwoWaySystemLinearMap<T, Storage> & a, Hash::Systems::TwoWaySystemLinearMap<T, Storage> & b) {
		a.swap(b);
	}

	template <typename T, class Storage>
	void swap(Hash::Systems::TwoWaySystemCWLFExponential<T, Storage> & a, Hash::Systems::TwoWaySystemCWLFExponential<T, Storage> & b) {
		a.swap(b);
	}

}

#endif /* TWO_WAY_SYSTEM_H */

