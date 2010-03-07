#include "default_test.h"

using namespace TestSuite;

// DefaultTest
DefaultTest::~DefaultTest(void) {
}

void DefaultTest::resetTest(void) {
	return this->results.resetTests();
}

const Assert & DefaultTest::getAssertionResult(void) const {
	return this->results;
}

bool DefaultTest::assertTrue(bool value, const std::string & message) {
	return this->results.assertTrue(value, message);
}

bool DefaultTest::assertFalse(bool value, const std::string & message) {
	return this->results.assertFalse(value, message);
}

bool DefaultTest::assertValid(void * ptr, const std::string & message) {
	return this->results.assertValid(ptr, message);
}

bool DefaultTest::assertNull(void * ptr, const std::string & message) {
	return this->results.assertNull(ptr, message);
}

// CompoundTest
CompoundTest::~CompoundTest(void) {
	for (TestsList::iterator i = this->tests.begin(), e = this->tests.end(); i != e; ++i) {
		delete (*i);
		(*i) = 0;
	}
}

void CompoundTest::setUp(void) {
}

void CompoundTest::tearDown(void) {
}

void CompoundTest::beforeTest(void) {
}

void CompoundTest::afterTest(void) {
}

void CompoundTest::addTest(Test * test) {
	this->tests.push_back(test);
}

void CompoundTest::resetTest(void) {
	this->results.resetTests();
}

void CompoundTest::runTest(void) {
	this->resetTest();

	this->setUp();

	const Assert * a = 0;
	for (TestsList::const_iterator i = this->tests.begin(), e = this->tests.end(); i != e; ++i) {
		this->beforeTest();
		(*i)->runTest();
		this->afterTest();

		a = &((*i)->getAssertionResult());
		this->results.addFailedCount(a->getFailedCount());
		this->results.addOkCount(a->getPassedCount());
	}

	this->tearDown();
}

const Assert & CompoundTest::getAssertionResult(void) const {
	return this->results;
}

// CTAssert
void CompoundTest::CTAssert::setOkCount(size_t count) {
	this->assertOk = count;
}

void CompoundTest::CTAssert::setFailedCoount(size_t count) {
	this->assertFailed = count;
}

void CompoundTest::CTAssert::addOkCount(size_t count) {
	this->assertOk += count;
}

void CompoundTest::CTAssert::addFailedCount(size_t count) {
	this->assertFailed += count;
}
