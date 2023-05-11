/**
 * @file looper.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 04 2022
 *
 * This is a header file implementing the Looper that servers as the main
 * control looping for the server
 */

#ifndef SRC_INCLUDE_CORE_LOOPER_H_
#define SRC_INCLUDE_CORE_LOOPER_H_

#include <atomic>
#include <cstdint>
#include <functional>
#include <future>  // NOLINT
#include <map>
#include <memory>
#include <mutex>  // NOLINT

#include "core/timer.h"
#include "core/utils.h"

namespace TURTLE_SERVER {

/* the epoll_wait time in milliseconds */
static constexpr int TIMEOUT = 3000;

/* a connection must be finished within this amount of time */
static constexpr uint64_t INACTIVE_TIMEOUT = 3000;

class Poller;

class ThreadPool;

class Connection;

class Acceptor;

/**
 * This Looper acts as the executor on a single thread
 * adopt the philosophy of 'one looper per thread'
 * */
class Looper {
 public:
  /* timer_expiration = 0 stands for no timer activated */
  explicit Looper(uint64_t timer_expiration = 0);

  ~Looper() = default;

  NON_COPYABLE(Looper);

  void Loop();

  void AddAcceptor(Connection *acceptor_conn);

  void AddConnection(std::unique_ptr<Connection> new_conn);

  auto RefreshConnection(int fd) noexcept -> bool;

  auto DeleteConnection(int fd) noexcept -> bool;

  void SetExit() noexcept;

 private:
  std::unique_ptr<Poller> poller_;
  std::mutex mtx_;
  std::map<int, std::unique_ptr<Connection>> connections_;
  std::map<int, Timer::SingleTimer *> timers_mapping_;
  Timer timer_{};
  bool exit_{false};
  bool use_timer_{false};
  uint64_t timer_expiration_{0};
};
}  // namespace TURTLE_SERVER
#endif  // SRC_INCLUDE_CORE_LOOPER_H_
