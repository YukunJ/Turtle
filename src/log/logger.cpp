/**
 * @file logger.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date March 22 2023
 *
 * This is an implementation file implementing the Singleton Logger class
 * for the logging purpose across Turtle system
 */

#include "log/logger.h"

namespace TURTLE_SERVER {

/*
 * public logging entry
 */
void Logger::LogMsg(LogLevel log_level, const std::string &msg) noexcept {}

/*
 * Singleton Pattern access point
 */
Logger &Logger::GetInstance() noexcept { return <#initializer #>; }

/*
 * private constructor
 * upon ctor, launch backend worker thread
 */
Logger::Logger() {}

/*
 * signal and harvest backend thread
 */
Logger::~Logger() {}

/*
 * internal helper to push a log into the FIFO queue
 * potentially notify the backend worker to swap and flush
 * if threshold criteria is met
 */
void Logger::PushLog(Logger::Log&& log) {}

/*
 * The thread routine for the backend log writer
 */
void Logger::LogWriting(Logger *logger) {}

}  // namespace TURTLE_SERVER
