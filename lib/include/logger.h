/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#ifndef FLOATINGPOINTHISTOGRAM_LOGGER_H
#define FLOATINGPOINTHISTOGRAM_LOGGER_H

#include <new>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

class Logger {
public:
  static std::shared_ptr<spdlog::logger> &getInstance() {
    static std::shared_ptr<spdlog::logger> logger = initializeLogger();
    return logger;
  }

private:
  static std::string getLogFileName() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M-%S");
    return "../logs/FloatingPointHistogram_" + ss.str() + ".log";
  }

  static std::shared_ptr<spdlog::logger> initializeLogger() {
    // Ensure the logs directory exists using std::filesystem
    std::filesystem::create_directories("../logs");

    // Create a new log file with a timestamp
    auto logger = spdlog::basic_logger_mt("file_logger", getLogFileName());

#ifdef DEBUG_MODE
    logger->set_level(spdlog::level::debug); // Log all levels, including debug
#else
    logger->set_level(spdlog::level::info); // Log info and higher levels
#endif

    // Set the log pattern
    logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");

    return logger;
  }

  // Disallow creating an instance of this object
  Logger() = default;
  ~Logger() = default;
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;
};

#define LOG_DEBUG(...)                                                         \
  Logger::getInstance()->debug(__VA_ARGS__) // Log a debug message
#define LOG_ERROR(...)                                                         \
  Logger::getInstance()->error(__VA_ARGS__) // Log an error message


#endif // FLOATINGPOINTHISTOGRAM_LOGGER_H
