#ifndef BASE_STEADY_TIME_TRAITS_HPP
#define BASE_STEADY_TIME_TRAITS_HPP

#include <chrono>
#include <ctime>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace apl {

/// Time traits specialised for posix_time.
struct steady_time_traits
{
	/// The time type.
//	typedef boost::posix_time::ptime time_type;
	typedef std::chrono::milliseconds time_type;

	/// The duration type.
	typedef boost::posix_time::time_duration duration_type;

	/// Get the current time.
	static time_type now()
	{
		timespec ts;
		if (clock_gettime(CLOCK_MONOTONIC, &ts))
		{
			// This should NEVER fail, if it does, we're royally screwed...
			assert(0);
		}

		// Stored in milliseconds accuracy.
		return time_type((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000));
	}

	/// Add a duration to a time.
	static time_type add(const time_type& t, const duration_type& d)
	{
		return t + time_type(d.total_milliseconds());
	}

	/// Subtract one time from another.
	static duration_type subtract(const time_type& t1, const time_type& t2)
	{
		const auto diff = t1 - t2;
		return boost::posix_time::milliseconds(diff.count());
	}

	/// Test whether one time is less than another.
	static bool less_than(const time_type& t1, const time_type& t2)
	{
		return t1 < t2;
	}

	/// Convert to POSIX duration type.
	static boost::posix_time::time_duration to_posix_duration(
			const duration_type& d)
	{
		return d;
	}
};

}

#endif // STEADY_BASE_TIME_TRAITS_HPP
