#ifndef SMART_POINTER_H
#define SMART_POINTER_H

#include <stdexcept>
#include <algorithm>
#include "hash_assert.h"

namespace Hash { namespace Utils {

	/** 
	 * Check policy for the SmartPointer - allows null referenced to be stored.
	 *
	 * @param T Type to be used in the smart pointer.
	 */
	template <typename T>
	struct NotNullCheckPolicy {
		/**
		 * Checks the pointer given in constructor.
		 *
		 * @param ptr A non-null pointer.
		 */
		static inline void newCheckPointer(const T * ptr) {
			if (ptr == 0) {
				throw std::invalid_argument("Referenced pointer may not be null.");
			}
		}

		/**
		 * Checks the pointer stored in a smart pointer.
		 *
		 * @param ptr A non-null pointer.
		 */
		static inline void referencedCheckPointer(const T * ptr) {
			simple_assert(ptr != 0, "R-value contained null.");
		}
	};

	/** 
	 * Check policy for the SmartPointer - does not allow null referenced to be stored.
	 *
	 * @param T Type to be used in the smart pointer.
	 */
	template <typename T>
	struct AllowNullCheckPolicy {
		/**
		 * Checks the pointer stored given in constructor. No checks
		 */
		static inline void newCheckPointer(const T *) {
		}

		/**
		 * Checks the pointer stored in a smart pointer. No checks
		 */
		static inline void referencedCheckPointer(const T *) {
		}
	};
	
	/** 
	 * Delete policy for the SmartPointer. Does not delete an object.
	 *
	 * @param T Type to be used in the smart pointer.
	 */
	template <typename T>
	struct NoDeleteObjectPolicy {
		/**
		 * Sets the pointer to 0.
		 *
		 * @param pointer Pointer to be deleted.
		 */
		static inline void deletePointer(T ** pointer) {
			*pointer = 0;
		}
	};

	/** 
	 * Delete policy for the SmartPointer. Deletes the object with the delete operator.
	 *
	 * @param T Type to be used in the smart pointer.
	 */
	template <typename T>
	struct DeleteObjectPolicy {
		/**
		 * Deletes the pointer. Sets it to 0.
		 *
		 * @param pointer Pointer to be deleted.
		 */
		static inline void deletePointer(T ** pointer) {
			delete (*pointer);
			*pointer = 0;
		}
	};

	/** 
	 * Delete policy for the SmartPointer. Deletes the pointer treating it as an array.
	 *
	 * @param T Type to be used in the smart pointer.
	 */
	template <typename T>
	struct DeleteArrayPolicy {
		/**
		 * Deletes the pointer. Sets it to 0.
		 *
		 * @param pointer Pointer to be deleted.
		 */
		static inline void deletePointer(T ** pointer) {
			delete [] (*pointer);
			*pointer = 0;
		}
	};

	/**
	 * Simple smart pointer - does reference counting and basic checks.
	 * This pointer holds only reference to valid objects (non null).
	 *
	 * @param T				Type to which the pointer is stored.
	 * @param CheckPolicy	Policy which does the non-null checking. The null pointer may be allowed or forbidden.
	 * @param DeletePolicy	Policy which does the pointer releasing. Use the appropriate policies for array, object or 
	 *						malloc allocated objects freeing. We may want to delete a pointer, do nothing or delete 
	 *						an array. This must be distinguished.
	 */
	template <
		typename T, 
		template <class> class CheckPolicy = NotNullCheckPolicy, 
		template <class> class DeletePolicy = DeleteObjectPolicy
	>
	class SmartPointer {
		/**
		 * This pointer type.
		 */
		typedef SmartPointer<T, CheckPolicy, DeletePolicy> ThisPointer;

		/**
		 * Null check policy used.
		 */
		typedef CheckPolicy<T> ThisCheckPolicy;

		/**
		 * Delete policy used.
		 */
		typedef DeletePolicy<T> ThisDeletePolicy;

		/**
		 * Swaps the contents of the two pointers.
		 *
		 * @param ptr1 The first pointer.
		 * @param ptr2 The second pointer.
		 */
		friend inline void swap(ThisPointer & ptr1, ThisPointer & ptr2) {
			std::swap(ptr1.refCounts, ptr2.refCounts);
			std::swap(ptr1.referencedObject, ptr2.referencedObject);	
		}

	public:
		/**
		 * Creates the pointer referring to the pointer. 
		 * This should be the only reference pointing to the object.
		 *
		 * @param ptr Object to be pointed to.
		 * @throw invalid_argument In case of the null ptr given.
		 */
		explicit SmartPointer(T * ptr):
			referencedObject(ptr)
		{
			ThisCheckPolicy::newCheckPointer(ptr);
			this->refCounts = new size_t;
			(*this->refCounts) = 1;
		}

		/**
		 * Releases the reference, possibly the whole object.
		 */
		~SmartPointer(void) {
			if (!--(*refCounts)) {
				ThisDeletePolicy::deletePointer(&referencedObject);
				simple_assert(referencedObject == 0, "Policy must zero the pointer.");

				delete refCounts;
				refCounts = 0;
			}
		}

		/**
		 * Copy c-tor.
		 *
		 * @param ptr Pointer to be copied.
		 */
		SmartPointer(const ThisPointer & ptr):
			referencedObject(ptr.referencedObject),
			refCounts(ptr.refCounts)
		{
			++(*this->refCounts);
		}

		/**
		 * Assignment operator.
		 *
		 * @param ptr Pointer to be used as the r-value.
		 */
		SmartPointer & operator =(const ThisPointer & ptr) {
			ThisCheckPolicy::referencedCheckPointer(ptr.referencedObject);
			ThisPointer tmp = ptr;
			swap(*this, tmp);
			return *this;
		}

		/**
		 * Tests if this pointer holds the only reference to the pointed object.
		 *
		 * @return True iff this is the only smart pointer pointing to the object.
		 */
		bool unique(void) const {
			return (*this->refCounts) == 1;
		}

		/**
		 * Returns instance that is referenced.
		 *
		 * @return Referenced instance.
		 */
		T * instance(void) {
			return this->referencedObject;
		}

		/**
		 * Returns instance that is referenced.
		 *
		 * @return Referenced instance.
		 */
		const T * instance(void) const {
			return (const_cast<ThisPointer *> (this))->instance();
		}

		/**
		 * -> emulation.
		 *
		 * @return The object that is pointed to.
		 */
		inline T * operator ->(void) {
			ThisCheckPolicy::referencedCheckPointer(this->referencedObject);
			return this->referencedObject;
		}

		/**
		 * -> emulation. Constant version.
		 *
		 * @return The object that is pointed to.
		 */
		inline const T * operator ->(void) const {
			return (const_cast<ThisPointer *> (this))->operator ->();
		}

		/**
		 * Dereference emulation.
		 *
		 * @return The object that is pointed to.
		 */
		inline T & operator *(void) {
			return *this->referencedObject;
		}

		/**
		 * Dereference emulation. Constant version.
		 *
		 * @return The object that is pointed to.
		 */
		inline const T & operator *(void) const {
			return *(const_cast<ThisPointer *> (this));
		}

		/**
		 * Bool operator.
		 *
		 * @return True if this is non null pointer. Should be always true!
		 */
		inline operator bool(void) const {
			return this->referencedObject != 0;
		}

		/**
		 * Negation of the bool operator.
		 *
		 * @return True if this is non null pointer. Should be always true!
		 */
		inline bool operator !(void) const {
			return this->referencedObject == 0;
		}

		/**
		 * Equality operator.
		 *
		 * @return True iff ptr1 and ptr2 referenced objects are the same.
		 */
		friend inline bool operator ==(const ThisPointer & ptr1, const ThisPointer & ptr2) {
			return ptr1.referencedObject == ptr2.referencedObject;
		}

		/**
		 * Inequality operator.
		 *
		 * @return True iff ptr1 and ptr2 referenced objects are different.
		 */
		friend inline bool operator !=(const ThisPointer & ptr1, const ThisPointer & ptr2) {
			return !(ptr1 == ptr2);
		}

		/**
		 * Less operator, does pointer comparison (as the natural pointers).
		 *
		 * @return True iff ptr1 referenced object pointer is less than ptr2 referenced object pointer.
		 */
		friend inline bool operator <(const ThisPointer & ptr1, const ThisPointer & ptr2) {
			return ptr1.referencedObject < ptr2.referencedObject;
		}

	private:
		/** 
		 * Reference counts.
		 */
		size_t * refCounts;

		/**
		 * Referenced object.
		 */
		T * referencedObject;
	};

} }

#endif /* SMART_POINTER_H */
