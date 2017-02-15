#include "hash_assert.h"
#include <string>
#include <sstream>

using namespace std;
using namespace Hash::Utils;

AssertException::AssertException(string message, string file, size_t line) {
	stringstream s;
	s << message << " [" << file << ":" << line << "]";
	this->message = s.str();
}

const char * AssertException::what(void) const throw() {
	return this->message.c_str();
}


AssertException::~AssertException(void) throw() {
}

void Hash::Utils::hash_assert(bool status, string message, string file, size_t line) {
	if (!status) {
		throw AssertException(message, file, line);
	}
}

#ifdef HASH_DEBUG
#ifdef __GNUC__

void simple_assert(bool status, string message) {
	Hash::Utils::hash_assert(status, message, "", 0);
}

#endif
#else

void simple_assert(bool status, string message) {
}

#endif
