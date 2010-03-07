#ifndef CWLF_SYSTEM_H
#define CWLF_SYSTEM_H

#include "utils/hash_assert.h"
#include "utils/rehash_observer.h"

namespace Hash {

	/**
	 * Carter - Wegman system of linear functions.
	 */
	template <typename T>
	class UniversalFunctionCWLF {
	public:
		typedef size_t HashType;
		
		UniversalFunctionCWLF(size_t length = START_LENGTH, size_t universumMax = START_UNIVERSUM_MAX) {
			this->setUniversumMax(universumMax);
			this->setLength(length);
			this->reset();
		}

		Hash::Utils::RehashObserver * getRehashObserver(void) {
			return new RehashObserver(this);
		}

		void setUniversumMax(size_t universumMax) {
			this->universumMax = universumMax;
		}

		size_t getUniversumMax(void) const {
			return this->universumMax;
		}

		void setLength(size_t length) {
			this->length = length;
		}

		size_t getLength(void) const {
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
			this->setLength(info.getTableSize());
			this->reset();
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

#endif /* CWLF_SYSTEM */
