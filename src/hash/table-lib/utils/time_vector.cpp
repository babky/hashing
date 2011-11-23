#include "utils/time_vector.h"

using namespace boost::posix_time;
using namespace std;
using namespace Hash::Utils;

void TimeVector::add(const time_duration & duration) {
	durations.push_back(duration);
}

time_duration TimeVector::getTotalTime(void) const {
	time_duration time;

	for (DurationVector::const_iterator b = durations.begin(), e = durations.end(); b != e; ++b) {
		time += *b;
	}

	return time;
}

boost::posix_time::time_duration TimeVector::getAverageTime(void) const {
	return getTotalTime() / size();
}

double TimeVector::getMillisVariance(void) const {
	double avg = getTotalTime().total_milliseconds() / (double) size();
	double sum = 0;
	double dif;

	for (DurationVector::const_iterator b = durations.begin(), e = durations.end(); b != e; ++b) {
		dif = (b->total_milliseconds() - avg);
		sum += dif * dif;
	}

	return sum / size();
}

size_t TimeVector::size(void) const {
	return durations.size();
}


TimeVector::DurationVector & TimeVector::getDurations(void) {
	return durations;
}
