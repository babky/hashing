#ifndef TWO_WAY_SYSTEM_H
#define TWO_WAY_SYSTEM_H

#include "utils/chain_length_aware_storage_info.h"
#include "systems/universal_system.h"
#include "utils/hash_assert.h"

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
		TwoWaySystem(void) {
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

} }

#endif /* TWO_WAY_SYSTEM_H */