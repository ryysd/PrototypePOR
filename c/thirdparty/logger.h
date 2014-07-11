#ifndef UTIL_LOGGER_H_
#define UTIL_LOGGER_H_


#include <iostream>
#include <cstdarg>
#include <cstdio>

struct log_level
{
    enum type
    {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };
};

std::ostream& operator<<(std::ostream& os, log_level::type level)
{
    switch (level) {
    case log_level::DEBUG: os << "DEBUG"; break;
    case log_level::INFO: os << "\x1b[34mINFO:\x1b[0m"; break;
    case log_level::WARN: os << "\x1b[33mWARN:\x1b[0m"; break;
    case log_level::ERROR: os << "\x1b[31mERROR:\x1b[0m"; break;
    case log_level::FATAL: os << "\x1b[31mFATAL:\x1b[0m"; break;
    }
    return os;
}

template <class Logger>
class log_info_with_logger
{
    const Logger& logger;
    const char* file_name;
    const int line_num;
    const log_level::type level;
public:
    log_info_with_logger(
        const Logger& logger,
        const char* file_name,
        int line_num,
        log_level::type level)
        : logger(logger), file_name(file_name), line_num(line_num), level(level)
    {}

    void operator()(const char* format, ...) const
    {
        std::va_list args;
        va_start(args, format);
        logger.put(file_name, line_num, level, format, args);
        va_end(args);
    }
};

class log_info
{
    const char* file_name;
    const int line_num;
    const log_level::type level;
public:
    log_info(const char* file_name, int line_num, log_level::type level)
        : file_name(file_name), line_num(line_num), level(level)
    {}

    template <class Logger>
    log_info_with_logger<Logger> with_logger(const Logger& logger) const
    {
        return log_info_with_logger<Logger>(logger, file_name, line_num, level);
    }
};

#define DEBUG log_info(__FILE__, __LINE__, log_level::DEBUG).with_logger(logger)
#define INFO log_info(__FILE__, __LINE__, log_level::INFO).with_logger(logger)
#define WARN log_info(__FILE__, __LINE__, log_level::WARN).with_logger(logger)
#define ERROR log_info(__FILE__, __LINE__, log_level::ERROR).with_logger(logger)
#define FATAL log_info(__FILE__, __LINE__, log_level::FATAL).with_logger(logger)

struct stdout_logger
{
    void put(const char* file_name, int line_num, log_level::type level, const char* format, std::va_list args) const
    {
        std::cout << level << " "
                  << file_name << "(" << line_num << "): ";
        std::vprintf(format, args);
        std::cout << std::endl;
    }
};

stdout_logger logger;

#endif
