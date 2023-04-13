/**
 * @file timer.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 11 2023
 *
 * This is a header file implementing the timer containing
 * a cluster of single timer
 * that's associated with an expiring time and callback func
 */

#ifndef SRC_INCLUDE_CORE_TIMER_H_
#define SRC_INCLUDE_CORE_TIMER_H_

#include <sys/timerfd.h>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>  // NOLINT
#include <vector>

namespace TURTLE_SERVER {

class Socket;
class Connection;

/*
 * helper to get current time in milliseconds since epoch
 */
auto NowSinceEpoch() noexcept -> uint64_t;

/*
 * helper to offset a timestamp as how far from now
 * must return a non-negative number
 */
auto FromNow(uint64_t timestamp) noexcept -> uint64_t;

/*
 * same as FromNow, but translate into the struct timespec
 */
auto FromNowInTimeSpec(uint64_t timestamp) noexcept -> struct timespec;

/*
 * helper to refresh the registered timer fd
 */
void ResetTimerFd(int timer_fd, struct timespec);

/*
 * Not thread-safe, designed to run in a single thread Looper class
 * One timer per Looper
 */
class Timer {
 public:
  class SingleTimer {
   public:
    SingleTimer(uint64_t expire_from_now, std::function<void()> callback) noexcept;

    auto WhenExpire() const noexcept -> uint64_t;

    auto Expired() const noexcept -> bool;

    void Run() noexcept;

   private:
    uint64_t expire_time_;
    std::function<void()> callback_{nullptr};
  };

  Timer();

  auto GetTimerConnection() -> Connection *;

  auto GetTimerFd() -> int;

  auto AddSingleTimer(uint64_t expire_from_now, const std::function<void()> &callback) noexcept -> SingleTimer *;

  auto RemoveSingleTimer(SingleTimer *single_timer) noexcept -> bool;

  auto NextExpireTime() const noexcept -> uint64_t;

  auto TimerCount() const noexcept -> size_t;

  auto PruneExpiredTimer() noexcept -> std::vector<std::unique_ptr<SingleTimer>>;

 private:
  struct SingleTimerCompartor {
    auto operator()(const SingleTimer *lhs, const SingleTimer *rhs) const noexcept -> bool;
  };

  void HandleRead();

  int timer_fd_;
  uint64_t next_expire_{0};
  mutable std::mutex mtx_;
  std::unique_ptr<Connection> timer_conn_;
  std::map<SingleTimer *, std::unique_ptr<SingleTimer>, SingleTimerCompartor> timer_queue_;
};

}  // namespace TURTLE_SERVER

#endif  // SRC_INCLUDE_CORE_TIMER_H_
