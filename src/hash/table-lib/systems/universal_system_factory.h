#ifndef UNIVERSAL_SYSTEM_FACTORY_H
#define UNIVERSAL_SYSTEM_FACTORY_H

namespace Hash { namespace Systems {

	/**
	 * Factory class for creating various Hash functions. Provide a partial specialization or full specialization in
	 * case of the system does not provide a standard constructor interface,
	 */
	template<typename T, class Storage, template <class, class> class HashFunction>
	class UniversalFunctionFactory {
	public:

		/**
		 * Method responsible for creating the hash function.
		 *
		 * @return Created hash function.
		 */
		static HashFunction<T, Storage> create(size_t size) {
			return HashFunction<T, Storage>(size);
		}

	};

} }


#endif /* UNIVERSAL_SYSTEM_FACTORY_H */
