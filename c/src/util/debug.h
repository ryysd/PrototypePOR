#ifndef UTIL_DEBUG_H_
#define UTIL_DEBUG_H_

#include <cstdio>
#include <cstdarg>

namespace debug {
#ifndef NODEBUG
  bool enable = false;

  namespace color {
    const char* normal  = "\x1B[0m";
    const char* red     = "\x1B[31m";
    const char* green   = "\x1B[32m";
    const char* yellow  = "\x1B[33m";
    const char* blue    = "\x1B[34m";
    const char* magenta = "\x1B[35m";
    const char* cyan    = "\x1B[36m";
    const char* white   = "\x1B[37m";
  };  // namespace color;

  void colored_printf(const char* color, const char* format, ...) {
    if (!enable) return;

    va_list arg;
    printf("%s", color);

    va_start(arg, format);
    vprintf(format, arg);
    va_end(arg);

    printf(color::normal);
  }

#define enable_debug() debug::enable = true
#define colored_dprint(color, fmt, ...) debug::colored_printf(color, fmt, ## __VA_ARGS__)
#define dprint(fmt, ...) colored_dprint(debug::color::normal, fmt, ## __VA_ARGS__)
#define dprint_warn(fmt, ...) colored_dprint(debug::color::yellow, "WARN: " fmt, ## __VA_ARGS__)
#define dprint_error(fmt, ...) colored_dprint(debug::color::red, "ERROR: " fmt, ## __VA_ARGS__)
#define dprint_info(fmt, ...) colored_dprint(debug::color::blue, "INFO: " fmt, ## __VA_ARGS__)
#define dprint_succ(fmt, ...) colored_dprint(debug::color::green, fmt, ## __VA_ARGS__)

#else
#define enable_debug()
#define dprint(fmt, ...)
#define dprint_warn(fmt, ...)
#define dprint_error(fmt, ...)
#define dprint_info(fmt, ...)

#endif  // NODEBUG
};  // namespace debug

#endif  // UTIL_DEBUG_H_
