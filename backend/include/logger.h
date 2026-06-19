#ifndef LOGGER_H
#define LOGGER_H

#include <string>

enum class LogLevel {
    INFO,
    WARN,
    ERROR
};

class Logger {
public:
    static void info(const std::string& message);
    static void warn(const std::string& message);
    static void error(const std::string& message);
    static void setLevel(LogLevel minLevel);

private:
    static LogLevel minLevel;
    static std::string getTimestamp();
    static void log(LogLevel level, const std::string& message);
    static const char* levelToString(LogLevel level);
};

#endif
