#ifndef ASSERT_H_TEST
#define ASSERT_H_TEST

#include <cstddef>
#include <string>
#include <ostream>

namespace TestSuite {

	/**
	 * Test result - assertion.
	 */
	class Assert {
	public:
		/**
		 * Clear result - no attempts, no failuers, no successes.
		 */
		Assert(void);

		/**
		 * Virtual destructor.
		 */ 
		virtual ~Assert(void);

		/**
		 * Resets the achieved results.
		 */
		virtual void resetTests(void);

		/**
		 * Passed tests count.
		 *
		 * @return Passed tests number.
		 */
		virtual size_t getPassedCount(void) const;

		/**
		 * Failed tests count.
		 *
		 * @return Failed tests number.
		 */
		virtual size_t getFailedCount(void) const;

		/**
		 * Tests count.
		 *
		 * @return Tests number.
		 */
		virtual size_t getTestsCount(void) const;

		/**
		 * Returns the results in the string.
		 *
		 * @return String containing the resultes description.
		 */
		virtual std::string getResults(void) const;

		// Possible asserts
		/** 
		 * Assumes the test result should be positive. Counts the result.
		 *
		 * @param value Test result.
		 * @param message Message to be shown in case of failure.
		 */
		virtual bool assertTrue(bool value, const std::string & message = "");

		/** 
		 * Assumes the test result should be negative. Counts the result.
		 *
		 * @param value Test result.
		 * @param message Message to be shown in case of failure.
		 */
		virtual bool assertFalse(bool value, const std::string & message = "");

		/** 
		 * Tests the pointer - should be non null.
		 *
		 * @param ptr Pointer to be tested.
		 * @param message Message to be shown in case of failure.
		 */
		virtual bool assertValid(void * ptr, const std::string & message = "");

		/** 
		 * Tests the pointer - should be null.
		 *
		 * @param ptr Pointer to be tested.
		 * @param message Message to be shown in case of failure.
		 */
		virtual bool assertNull(void * ptr, const std::string & message = "");

		/** 
		 * Checks the two values for equality.
		 *
		 * @param T Type of the compared values.
		 * @param v1 The first value.
		 * @param v2 The second value.
		 * @param message Message to be shown in case of failure.
		 */
		template <typename T>
		bool assertEqual(const T & v1, const T & v2, const std::string & message = "")
		{
			return assertTrue(v1 == v2, message);
		}

	protected:
		/** 
		 * Counts the test result (assert).
		 *
		 * @param result True if the test was fine, false if failed.
		 * @param message Message to be shown in case of failure.
		 */
		bool testResult(bool result, const std::string & message);

		/**
		 * Passed tests count.
		 */
		size_t assertOk;

		/**
		 * Failed tests count.
		 */
		size_t assertFailed;
	};

	/**
	 * Result printing.
	 *
	 * @param out Output stream to which we are printing.
	 * @param assert Assertion results being printed.
	 * @return Used stream.
	 */
	std::ostream & operator <<(std::ostream & out, const Assert & assert);
}

#endif /* ASSERT_H_TEST */
