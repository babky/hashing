#ifndef UNIVERSAL_SYSTEM_H
#define UNIVERSAL_SYSTEM_H

namespace Hash { namespace Systems {

	/**
	 * Abstraction of a system of universal functions.
	 */
	template<typename T>
	class UniversalSystem {
	public:
		virtual void reset(void) = 0;

		virtual size_t getLength(void) const = 0;

		virtual void setLength(size_t length) = 0;

		virtual size_t getUniversumMax(void) const = 0;

		virtual void setUniversumMax(size_t universumMax) = 0;

		virtual size_t hash(const T & x, size_t length) = 0;

		virtual size_t operator()(const T & a, size_t length) = 0;
	};

} }

#endif /* UNIVERSAL_SYSTEM_H */