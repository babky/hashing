#ifndef FUNCTION_H
#define FUNCTION_H

#include <storage.h>
#include <algorithm>

namespace Hash {

	// TODO: Inherit from UniversalFunction

	/**
	 * Default hashing function.
	 */
	template<typename T, class Storage>
	class Function {
	public:
		typedef size_t HashType;

		void setTableSize(size_t aTableSize) {
			tableSize = aTableSize;
		}

		void reset(void) {
		}

		size_t hash(const T & a) {
			return a % tableSize;
		}

		size_t operator()(const T & a) {
			return hash(a);
		}

		void swap(Function & b) {
			using std::swap;

			swap(tableSize, b.tableSize);
		}

		void setStorage(Storage *) {
		}

	private:
		size_t tableSize;
	};

}

namespace std {
		
	template <typename T, class Storage>
	void swap(Hash::Function<T, Storage> & a, Hash::Function<T, Storage> & b) {
		Hash::Function<T, Storage> tmp = a;
		a = b;
		b = tmp;
	}

}

#endif /* FUNCTION_H */