#ifndef UNIVERSAL_SYSTEM_H
#define UNIVERSAL_SYSTEM_H

namespace Hash { namespace Systems {

	/**
	 * Abstraction of a system of universal functions.
	 * Also note that each of the systems must provide the definition HashType - type of the hash value returned by the
	 * system.
	 *
	 * @typeparam T The type of the hashed values.
	 */
	template<typename T>
	class UniversalSystem {
	public:
		/**
		 * Initializes the system and adapts it to the given storage.
		 *
		 * @param info Storage information according to which the system should be adapted.
		 */
		virtual void initialize(Hash::Utils::ChainLengthAwareStorageInfo & info) = 0;

		/**
		 * Resets the function - chooses another from the universal system.
		 */
		virtual void reset(void) = 0;

		/**
		 * Current length of the hash table.
		 *
		 * @return The length of the hash table which is supposed by the current hash function.
		 */
		virtual size_t getLength(void) const = 0;
		
		/**
		 * Sets the length of the table which will be used by the current hash function.
		 *
		 *
		 * @param length New length of the table.
		 */
		virtual void setLength(size_t length) = 0;

		/**
		 * Maximum value of the universe.
		 *
		 * @param Maximal possible value.
		 */
		virtual size_t getUniversumMax(void) const = 0;

		/**
		 * Maximum value of the universe.
		 *
		 * @param Maximal possible value.
		 */
		virtual void setUniversumMax(size_t universumMax) = 0;

		/**
		 * Hash value of the element x
		 *
		 * @param x The hashed element.
		 * @param length Length of the table.
		 */
		virtual size_t hash(const T & x, size_t length) = 0;

		/**
		 * Hash value of the element x
		 *
		 * @param x The hashed element.
		 * @param length Length of the table.
		 */
		virtual size_t operator()(const T & a, size_t length) = 0;
	};

} }

#endif /* UNIVERSAL_SYSTEM_H */