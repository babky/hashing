#ifndef TWO_WAY_SYSTEM_H
#define TWO_WAY_SYSTEM_H

#include "utils/chain_length_aware_storage_info.h"
#include "systems/universal_system.h"
#include "utils/hash_assert.h"

namespace Hash { namespace Systems {

	/**
	 * System allowing two way universal hash function.
	 */
	template <typename T, template <typename> class System>
	class TwoWaySystem {
	public:
		typedef System<T> Function;

		typedef size_t HashType;

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

		virtual size_t getLength(void) const {
			simple_assert(f.getLength() == g.getLength(), "Lengths for the two functions must be the same.");
			return f.getLength();
		}

		virtual void setLength(size_t length) {
			f.setLength(length);
			g.setLength(length);
		}

		virtual size_t getUniversumMax(void) const {
			simple_assert(f.getLength() == g.getLength(), "Universum sizes the two functions must be the same.");
			return f.getUniversumMax();
		}

		virtual void setUniversumMax(size_t universumMax) {
			f.setUniversumMax(universumMax);
			g.setUniversumMax(universumMax);
		}

		virtual HashType hash(const T & x, size_t length) {
			HashType hf = f.hash(x, length);
			HashType hg = g.hash(x, length);

			return storage->getChainLength(hf) < storage->getChainLength(hg) ? hf : hg;
		}

		virtual HashType operator()(const T & a, size_t length) {
			return hash(a, length);
		}

	private:
		Function f;
		Function g;
		Utils::ChainLengthAwareStorageInfo * storage;
	};

} }

#endif /* TWO_WAY_SYSTEM_H */