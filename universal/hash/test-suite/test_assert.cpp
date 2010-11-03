#include "test_assert.h"
#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;
using namespace TestSuite;

// Assert
Assert::Assert(void):
	assertOk(0),
	assertFailed(0)
{
}

Assert::~Assert(void) {
}

size_t Assert::getPassedCount(void) const {
	return this->assertOk;
}

size_t Assert::getFailedCount(void) const {
	return this->assertFailed;
}

size_t Assert::getTestsCount(void) const {
	return this->assertOk + this->assertFailed;
}

std::string Assert::getResults(void) const {
	stringstream ss;
	ss << "Tests count: " << this->getTestsCount() << "\n";
	ss << "Passed: " << this->getPassedCount() << "\n";
	ss << "Failed: " << this->getFailedCount() << "\n";
	return ss.str();;
}

void Assert::resetTests(void) {
	this->assertOk = 0;
	this->assertFailed = 0;
}

bool Assert::assertTrue(bool value, const std::string & message) {
	return testResult(value, message);
}

bool Assert::assertFalse(bool value, const std::string & message) {
	return testResult(!value, message);
}

bool Assert::assertValid(void * ptr, const std::string & message) {
	return testResult(ptr != 0, message);
}

bool Assert::assertNull(void * ptr, const std::string & message) {
	return testResult(ptr == 0, message);
}

bool Assert::testResult(bool result, const std::string & message) {
	if (result) {
		++this->assertOk;
	} else {
		if (!message.empty()) {
			cerr << message << "\n";
		}
		++this->assertFailed;
	}

	return result;
}

ostream & TestSuite::operator <<(ostream & out, const Assert & assert) {
	return out << assert.getResults();
}
