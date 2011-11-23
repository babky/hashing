#ifndef TIME_VECTOR_H
#define TIME_VECTOR_H

#include <vector>
#include <ostream>
#include "utils/boost_include.h"

namespace Hash { namespace Utils {

	/**
	 * The class responsible for handling time results.
	 */
	class TimeVector {
	public:
		typedef std::vector<boost::posix_time::time_duration> DurationVector;

		void add(const boost::posix_time::time_duration & duration);
		boost::posix_time::time_duration getTotalTime(void) const;
		boost::posix_time::time_duration getAverageTime(void) const;
		double getMillisVariance(void) const;
		size_t size(void) const;
		DurationVector & getDurations(void);
		const DurationVector & getDurations(void) const;

	protected:
		DurationVector durations;
	};

	std::ostream & operator <<(std::ostream & out, const TimeVector & vector);

} }

#endif /* TIME_VECTOR_H */
