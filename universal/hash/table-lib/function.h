#ifndef FUNCTION_H
#define FUNCTION_H

#include <storage.h>

namespace Hash {

	/**
	 * Default hashing function.
	 */
	template<typename T>
	class Function {
	public:

		typedef size_t HashType;

		size_t hash(const T & a, size_t length) {
			return a % length;
		}

		size_t operator()(const T & a, size_t length) {
			return hash(a, length);
		}

		void initialize(Hash::StorageInfo &) {
		}

	};

}

#endif /* FUNCTION_H */