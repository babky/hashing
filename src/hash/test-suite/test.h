#ifndef TEST_H
#define TEST_H

#include "test_assert.h"

namespace TestSuite
{

	/**
	 * Basic interface for testing.
	 */
	class Test
	{
	public:
		/**
		 * Constructor.
		 */
		Test(void);

		/**
		 * Virtual destructor.
		 */
		virtual ~Test(void);

		/**
		 * Restarts the test results.
		 */
		virtual void resetTest(void) = 0;

		/**
		 * Runs the test.
		 */
		virtual void runTest(void) = 0;

		/**
		 * Test result.
		 *
		 * @return Assertion result.
		 */
		virtual const Assert & getAssertionResult(void) const = 0;

	private:
		/**
		 * Forbidden copy constructor.
		 */
		Test(const Test &);

		/**
		 * Assignment operator is prohibited.
		 */
		Test & operator =(const Test &);
	};

}

#endif /* TEST_H */
