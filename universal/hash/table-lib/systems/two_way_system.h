#ifndef TWO_WAY_SYSTEM_H
#define TWO_WAY_SYSTEM_H

#include "utils/chain_length_aware_storage_info.h"
#include "systems/linear_map_system.h"
#include "systems/polynomial_system.h"
#include "systems/universal_system.h"
#include "utils/hash_assert.h"
#include <algorithm>

namespace Hash { namespace Systems {
	
	/**
	 * System allowing two way universal hash function. The value is hashed to the shorter chain.
	 *
	 * @typeparam T Type of the hashed values.
	 * @typeparam System The universal system which will be used.
	 */
	template <typename T, template <typename> class System>
	class TwoWaySystem : public UniversalSystem<T> {
	public:
		// Use the former HashType;
		using UniversalSystem<T>::HashType;
	
		/**
		 * Single function type, it is chosen randomly from the given universal system.
		 */
		typedef System<T> Function;

		/**
		 * C-tor.
		 */
		TwoWaySystem(void):
		  storage(0) {
			reset();
		}

		void initialize(Hash::Utils::ChainLengthAwareStorageInfo & info) {
			f.initialize(info);
			g.initialize(info);
			storage = &info;
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

		virtual T getUniversumMax(void) const {
			simple_assert(f.getUniversumMax() == g.getUniversumMax(), "Universum sizes the two functions must be the same.");
			return f.getUniversumMax();
		}

		virtual void setUniversumMax(T universumMax) {
			f.setUniversumMax(universumMax);
			g.setUniversumMax(universumMax);
		}

		virtual size_t hash(const T & x, size_t length) {
			size_t hf = f.hash(x, length);
			size_t hg = g.hash(x, length);

			return storage->getChainLength(hf) < storage->getChainLength(hg) ? hf : hg;
		}

		virtual size_t operator()(const T & a, size_t length) {
			return hash(a, length);
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
		 * Currently used storage.
		 */
		Utils::ChainLengthAwareStorageInfo * storage;
	};

	/**
	 * The possibility of two way linear hashing.
	 */
	template <typename T>
	class TwoWaySystemLinearMap : public Hash::Systems::TwoWaySystem<T, Hash::UniversalFunctionLinearMap> {
	};

	/**
	 * The possibility of two way polynomial hashing.
	 */
	template <typename T>
	class TwoWaySystemPolynomial : public Hash::Systems::TwoWaySystem<T, Hash::Systems::PolynomialSystem> {
	};

} }

namespace std {
		
	template <typename T, template <typename> class System>
	void swap(Hash::Systems::TwoWaySystem<T, System> & a, Hash::Systems::TwoWaySystem<T, System> & b) {
		a.swap(b);
	}	
	
	template <typename T>
	void swap(Hash::Systems::TwoWaySystemLinearMap<T> & a, Hash::Systems::TwoWaySystemLinearMap<T> & b) {
		a.swap(b);
	}

}

#endif /* TWO_WAY_SYSTEM_H */

