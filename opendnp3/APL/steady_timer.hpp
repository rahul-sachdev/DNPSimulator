#ifndef BASE_STEADY_TIMER_HPP
#define BASE_STEADY_TIMER_HPP

#include <boost/asio/basic_deadline_timer.hpp>
#include "steady_time_traits.hpp"

namespace apl {

typedef boost::asio::basic_deadline_timer<std::chrono::milliseconds, steady_time_traits> steady_timer;

}

#endif // BASE_STEADY_TIMER_HPP
