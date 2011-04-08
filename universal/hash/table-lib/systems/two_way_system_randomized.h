#ifndef TWO_WAY_SYSTEM_RANDOMIZED_H
#define TWO_WAY_SYSTEM_RANDOMIZED_H

#include "two_way_system.h"
#include "utils/random_generator.h"
#include <boost/thread.hpp>
#include "utils/smart_pointer.h"

namespace Hash { namespace Systems {
	
	/**
	 * System allowing two way universal hash function. The value is hashed to the shorter chain in one of the two 
	 * randomly selected two way groups.
	 *
	 * @typeparam T Type of the hashed values.
	 * @typeparam System1 The two way system which will be used.
	 * @typeparam System2 The two way system which will be used.
	 */
	template <typename T, template <typename> class System1, template <typename> class System2>
	class TwoWaySystemRandomized : public UniversalSystem<T> {
	public:
		TwoWaySystemRandomized(void):
		  bitGenerator(new Hash::Utils::RandomGenerator<size_t>(0, boost::integer_traits<size_t>::const_max, true)),
		  mutex(new boost::mutex()),
		  p(REGENERATE_NOW) {
		}
		
		void initialize(Hash::Utils::ChainLengthAwareStorageInfo & info) {
			group1.initialize(info);
			group2.initialize(info);
		}

		virtual void reset(void) {
			group1.reset();
			group2.reset();
		}

		virtual size_t getTableSize(void) const {
			simple_assert(group1.getTableSize() == group2.getTableSize(), "Lengths for the two function groups must be the same.");
			return group1.getTableSize();
		}
		
		virtual void setTableSize(size_t size) {
			group1.setTableSize(size);
			group2.setTableSize(size);
		}

		virtual T getUniversumMax(void) const {
			simple_assert(group1.getUniversumMax() == group2.getUniversumMax(), "Universum maximums must be the same.");
			return group1.getUniversumMax();
		}

		virtual void setUniversumMax(T universumMax) {
			group1.setUniversumMax(universumMax);
			group2.setUniversumMax(universumMax);
		}

		virtual size_t hash(const T & x, size_t length) {
			size_t randomBit;
			mutex->lock();
			if (p == REGENERATE_NOW) {
				p = 1;
				randomWord = bitGenerator->generate();
				randomBit = randomWord & p;
			} else {
				randomBit = randomWord & p;
			}

			p <<= 1;
			mutex->unlock();

			if (randomBit) {
				return group1.hash(x, length);
			} else {
				return group2.hash(x, length);
			}
		}

		virtual size_t operator()(const T & a, size_t length) {
			return hash(a, length);
		}

		void swap(TwoWaySystemRandomized & b) {
			using std::swap;

			swap(bitGenerator, b.bitGenerator);
			swap(mutex, b.mutex);
			group1.swap(b.group1);
			group2.swap(b.group2);
		}

		friend void swap(TwoWaySystemRandomized & a, TwoWaySystemRandomized & b) {
			a.swap(b);
		}

	private:
		Hash::Utils::SmartPointer<Hash::Utils::RandomGenerator<size_t> > bitGenerator;
		Hash::Utils::SmartPointer<boost::mutex> mutex;
		size_t randomWord;
		size_t p;

		static const size_t REGENERATE_NOW = ((size_t) 1) << (boost::integer_traits<size_t>::digits - 1);

		System1<T> group1;
		System2<T> group2;
	};

	template <typename T>
	class TwoWaySystemRandomizedLinearMap : public TwoWaySystemRandomized<T, TwoWaySystemLinearMap, TwoWaySystemLinearMap> {
	};

} }

namespace std {
		
	template <typename T, template <typename> class System1, template <typename> class System2>
	void swap(Hash::Systems::TwoWaySystemRandomized<T, System1, System2> & a, Hash::Systems::TwoWaySystemRandomized<T, System1, System2> & b) {
		a.swap(b);
	}	
	
}

#endif /* TWO_WAY_SYSTEM_RANDOMIZED_H */
