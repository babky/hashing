#ifndef CWLF_SYSTEM_H
#define CWLF_SYSTEM_H

#include <algorithm>
#include "systems/universal_system.h"
#include "utils/hash_assert.h"
#include "utils/rehash_observer.h"
#include "utils/static_random_generator.h"

namespace Hash {

	/**
	 * Carter - Wegman system of linear functions.
	 */
	template <typename T>
	class UniversalFunctionCWLF : public Hash::Systems::UniversalSystem<T> {
	public:
		explicit UniversalFunctionCWLF(size_t length = START_LENGTH, size_t universumMax = START_UNIVERSUM_MAX) {
			this->setUniversumMax(universumMax);
			this->setTableSize(length);
			this->reset();
		}

		Hash::Utils::RehashObserver * getRehashObserver(void) {
			return new RehashObserver(this);
		}

		void setUniversumMax(T universumMax) {
			this->universumMax = universumMax;
		}

		T getUniversumMax(void) const {
			return this->universumMax;
		}

		void setTableSize(size_t size) {
			this->length = size;
		}

		size_t getTableSize(void) const {
			return this->length;
		}

		void reset(void) {
			Hash::Utils::IntegralGeneratorWrapper<size_t> g =
				Hash::Utils::IntegralGeneratorWrapper<size_t>(0, this->getUniversumMax());

			this->a = g.generate();
			this->b = g.generate();
		}

		size_t hash(const T & x, size_t length) {
			simple_assert(this->length == length, "CWLF system table's size differs from the wanted size.");
			return ((this->a * x + this->b) % this->universumMax) % this->length;
		}

		size_t operator()(const T & a, size_t length) {
			return hash(a, length);
		}

		void initialize(Hash::StorageInfo & info) {
			this->setTableSize(info.getTableSize());
			this->reset();
		}

		void swap(UniversalFunctionCWLF<T> & system) {
			std::swap(universumMax, system.universumMax);
			std::swap(a, system.a);
			std::swap(b, system.b);
			std::swap(length, system.length);
		}

	private:
		class RehashObserver : public Hash::Utils::RehashObserver {
		public:
			RehashObserver(UniversalFunctionCWLF<T> * function):
			  f(function){
			}

			void rehash(Hash::StorageInfo & info) {
				this->f->initialize(info);
			}
				
		private:
			UniversalFunctionCWLF<T> * f;
		};

		const static size_t START_UNIVERSUM_MAX = 65537;
		const static size_t START_LENGTH = 10;

		size_t universumMax;
		size_t a, b;
		size_t length;
	};

}

namespace std {

	template <typename T>
	void swap(Hash::UniversalFunctionCWLF<T> & a, Hash::UniversalFunctionCWLF<T> & b) {
		a.swap(b);
	}

}

#endif /* CWLF_SYSTEM */
