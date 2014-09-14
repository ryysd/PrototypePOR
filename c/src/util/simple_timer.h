#ifndef UTIL_SIMPLE_TIMER_H_
#define UTIL_SIMPLE_TIMER_H_

#include <chrono>  // NOLINT

namespace simpletimer {
#ifndef NODEBUG
  std::chrono::time_point<std::chrono::system_clock> start_time;

#define simpletimer_start() (simpletimer::start_time = std::chrono::system_clock::now())
#define simpletimer_stop() (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - simpletimer::start_time).count())
#else

#define simple_timer_start()
#define simple_timer_stop()
#endif
};  // namespace simpletimer

#endif  // UTIL_SIMPLE_TIMER_H_
