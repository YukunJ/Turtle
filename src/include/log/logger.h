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

#ifndef SRC_INCLUDE_LOG_LOGGER_H_
#define SRC_INCLUDE_LOG_LOGGER_H_

#include <atomic>
#include <chrono>              // NOLINT
#include <condition_variable>  // NOLINT
#include <deque>
#include <iomanip>
#include <iostream>
#include <mutex>  // NOLINT
#include <sstream>
#include <string>
#include <thread>  // NOLINT
#include "core/utils.h"

namespace TURTLE_SERVER {

enum class LogLevel { INFO, WARNING, ERROR, FATAL };

/* mapping LogLevel enum to string representation */
const char *log_level_names[] = {"INFO: ", "WARNING: ", "ERROR: ", "FATAL: "};

constexpr int INIT_CAPACITY = 1000;

/* threshold */
constexpr int COUNT_THRESHOLD = 100;
constexpr uint64_t REFRESH_THRESHOLD = 3000;

/**
 * A simple asynchronous logger
 * All callers counts as frontend-producer and is non-blocking
 * a backend worker thread periodically flush the log to persistent storage
 */
class Logger {
  /*
   * Each individual Log message
   * upon construction, the date time is prepended
   */
  struct Log {
    std::string stamped_msg;

    /* stamp datetime and log level */
    Log(const LogLevel log_level, const std::string &log_msg) noexcept {
      auto t = std::time(nullptr);
      auto tm = *std::localtime(&t);
      std::ostringstream stream;
      stream << std::put_time(&tm, "[%d %b %Y %H:%M:%S]") << log_level_names[static_cast<int>(log_level)] << log_msg
             << std::endl;
      stamped_msg = stream.str();
    }
  };

 public:
  /*
   * public logging entry
   */
  static void LogMsg(LogLevel log_level, const std::string &msg) noexcept;

  /*
   * Singleton Pattern access point
   */
  static Logger &GetInstance() noexcept;

  NON_COPYABLE_AND_MOVEABLE(Logger);

 private:
  /*
   * private constructor
   * upon ctor, launch backend worker thread
   */
  Logger();

  /*
   * signal and harvest backend thread
   */
  ~Logger();

  /*
   * internal helper to push a log into the FIFO queue
   * potentially notify the backend worker to swap and flush
   * if threshold criteria is met
   */
  void PushLog(Log&& log);

  /*
   * The thread routine for the backend log writer
   */
  static void LogWriting(Logger *logger);

  std::atomic<bool> done_ = false;
  std::mutex mtx_;
  std::condition_variable cv_;
  std::deque<Log> queue_;
  std::thread log_writer_;
  uint64_t last_flush_;
};

}  // namespace TURTLE_SERVER

#endif  // SRC_INCLUDE_LOG_LOGGER_H_
