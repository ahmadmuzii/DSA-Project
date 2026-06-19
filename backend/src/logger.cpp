#include "logger.h"
#include <iostream>
#include <ctime>

LogLevel Logger::minLevel = LogLevel::INFO;

void Logger::setLevel(LogLevel level) {
    minLevel = level;
}

std::string Logger::getTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[20];
    if (std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&now))) {
        return buf;
    }
    return "??:??:??";
}

const char* Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        default:              return "????";
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < minLevel) return;

    std::cout << "[" << getTimestamp() << "]"
              << " [" << levelToString(level) << "]"
              << " " << message << std::endl;
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warn(const std::string& message) {
    log(LogLevel::WARN, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}
