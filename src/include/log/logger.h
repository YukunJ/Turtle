/**
 * @file logger.h
 * @author Yukun J
 * @expectation this file should be compatible to compile in C++
 * program on Linux
 * @init_date March 22 2023
 *
 * This is a header file implementing the Singleton Logger class
 * for the logging purpose across Turtle system
 */
#pragma once

#ifndef SRC_INCLUDE_LOG_LOGGER_H_
#define SRC_INCLUDE_LOG_LOGGER_H_

#include <algorithm>
#include <atomic>
#include <chrono>              // NOLINT
#include <condition_variable>  // NOLINT
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>  // NOLINT
#include <sstream>
#include <string>
#include <thread>  // NOLINT
#include "core/utils.h"

namespace TURTLE_SERVER {

enum class LogLevel { INFO, WARNING, ERROR, FATAL };

/* threshold */
constexpr int COUNT_THRESHOLD = 1000;
constexpr std::chrono::duration REFRESH_THRESHOLD = std::chrono::microseconds(3000);

/* log file name if used */
const std::string LOG_PATH = std::string("TurtleLog");  // NOLINT

/**
 * A simple asynchronous logger
 * All callers counts as frontend-producer and is non-blocking
 * a backend worker thread periodically flush the log to persistent storage
 */
class Logger {
 public:
  /*
   * public logging entry
   */
  static void LogMsg(LogLevel log_level, const std::string &msg) noexcept;

  /*
   * Singleton Pattern access point
   */
  static auto GetInstance() noexcept -> Logger &;

  NON_COPYABLE_AND_MOVEABLE(Logger);

  /*
   * Each individual Log message
   * upon construction, the date time is prepended
   */
  struct Log {
    std::string stamped_msg_;

    /*
     stamp datetime and log level
     not guaranteed to be output in the stamped time order, best effort approach
    */
    Log(LogLevel log_level, const std::string &log_msg) noexcept;

    friend auto operator<<(std::ostream &os, const Log &log) -> std::ostream & {
      os << log.stamped_msg_;
      return os;
    }
  };

 private:
  /*
   * private constructor, takes in a logging strategy
   * upon ctor, launch backend worker thread
   */
  explicit Logger(const std::function<void(const std::deque<Log> &logs)> &log_strategy);

  /*
   * signal and harvest backend thread
   */
  ~Logger();

  /*
   * internal helper to push a log into the FIFO queue
   * potentially notify the backend worker to swap and flush
   * if threshold criteria is met
   */
  void PushLog(Log &&log);

  /*
   * The thread routine for the backend log writer
   */
  void LogWriting();

  std::function<void(const std::deque<Log> &)> log_strategy_;
  std::atomic<bool> done_ = false;
  std::mutex mtx_;
  std::condition_variable cv_;
  std::deque<Log> queue_;
  std::thread log_writer_;
  std::chrono::microseconds last_flush_;
};

/* macro definitions for 4 levels of logging */
#ifdef NOLOG
#define LOG_INFO(x) {};
#define LOG_WARNING(x) {};
#define LOG_ERROR(x) {};
#define LOG_FATAL(x) {};
#else
#define LOG_INFO(x) TURTLE_SERVER::Logger::LogMsg(TURTLE_SERVER::LogLevel::INFO, (x));
#define LOG_WARNING(x) TURTLE_SERVER::Logger::LogMsg(TURTLE_SERVER::LogLevel::WARNING, (x));
#define LOG_ERROR(x) TURTLE_SERVER::Logger::LogMsg(TURTLE_SERVER::LogLevel::ERROR, (x));
#define LOG_FATAL(x) TURTLE_SERVER::Logger::LogMsg(TURTLE_SERVER::LogLevel::FATAL, (x));
#endif
}  // namespace TURTLE_SERVER

#endif  // SRC_INCLUDE_LOG_LOGGER_H_
