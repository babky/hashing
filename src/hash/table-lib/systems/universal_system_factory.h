#ifndef UNIVERSAL_SYSTEM_FACTORY_H
#define UNIVERSAL_SYSTEM_FACTORY_H

namespace Hash { namespace Systems {

	/**
	 * Factory class for creating various Hash functions. Provide a partial specialization or full specialization in
	 * case of the system does not provide a standard constructor interface,
	 */
	template<typename T, template <class> class HashFunction>
	class UniversalSystemFactory {
	public:

		/**
		 * Method responsible for creating the hash function.
		 *
		 * @return Created hash function.
		 */
		static HashFunction<T> create(size_t size) {
			return HashFunction<T>(size);
		}

	};

} }


#endif /* UNIVERSAL_SYSTEM_FACTORY_H */
