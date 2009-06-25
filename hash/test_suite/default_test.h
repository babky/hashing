#ifndef DEFAULT_TEST_H
#define DEFAULT_TEST_H

#include "Test.h"
#include <vector>
#include <string>

namespace TestSuite {

	class DefaultTest : public Test {
	public:
		virtual ~DefaultTest(void);
		virtual void resetTest(void);
		virtual const Assert & getAssertionsResult(void) const;

	protected:
		virtual bool assertTrue(bool value, const std::string & message = "");
		virtual bool assertFalse(bool value, const std::string & message = "");
		virtual bool assertValid(void * ptr, const std::string & message = "");
		virtual bool assertNull(void * ptr, const std::string & message = "");
		template <typename T>
		bool assertEqual(const T & v1, const T & v2, const std::string & message = "")
		{
			return this->assertTrue(v1 == v2, message);
		}

	private:
		Assert results;
	};

	class CompoundTest : public Test {
	public:
		virtual ~CompoundTest(void);

		virtual void setUp(void);
		virtual void tearDown(void);

		virtual void beforeTest(void);
		virtual void afterTest(void);

		void addTest(Test * test);

		virtual void resetTest(void);
		virtual void runTest(void);
		virtual const Assert & getAssertionsResult(void) const;

	protected:
		class CTAssert : public Assert {
		public:
			virtual void setOkCount(size_t count);
			virtual void setFailedCoount(size_t count);

			virtual void addOkCount(size_t count);
			virtual void addFailedCount(size_t count);
		};
		
	private:
		typedef std::vector<Test *> TestsList;
		TestsList tests;
		CTAssert results;
	};

}

#endif /* DEFAULT_TEST_H */
