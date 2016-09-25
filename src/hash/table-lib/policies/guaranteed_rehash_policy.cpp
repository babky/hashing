#include "guaranteed_rehash_policy.h"
#include <cmath>

using namespace Hash;
using namespace Hash::Policies;
using namespace Hash::Policies::Rehash;
using namespace Hash::Utils;
using namespace std;

double log2(double n) {
	return log(static_cast<double> (n)) / log(static_cast<double> (2));
}

const double GuaranteedRehashPolicy::MIN_LOAD_FACTOR_NO_DELETE = 0.5;
const double GuaranteedRehashPolicy::MAX_LOAD_FACTOR_NO_DELETE = 1;
const double GuaranteedRehashPolicy::MULTIPLICATIVE_CONSTANT_NO_DELETE = 19.152;

const double GuaranteedRehashPolicy::MIN_LOAD_FACTOR_ALLOW_DELETE = 0.25;
const double GuaranteedRehashPolicy::MAX_LOAD_FACTOR_ALLOW_DELETE = 1;
const double GuaranteedRehashPolicy::MULTIPLICATIVE_CONSTANT_ALLOW_DELETE = 28.728;

const size_t GuaranteedRehashPolicy::MINIMAL_ALLOWED_BOUND = 512;
