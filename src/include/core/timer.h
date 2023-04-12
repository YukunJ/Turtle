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

#include <cstdint>
#include <functional>
#include <map>
#include <memory>

namespace TURTLE_SERVER {

/*
 * helper to get current time in milliseconds since epoch
 */
auto NowSinceEpoch() noexcept -> uint64_t;
class Timer;

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

  auto AddSingleTimer(uint64_t expire_from_now, const std::function<void()> &callback) noexcept -> SingleTimer *;

  auto RemoveSingleTimer(SingleTimer *single_timer) noexcept -> bool;

  auto NextExpireTime() const noexcept -> uint64_t;

  auto TimerCount() const noexcept -> size_t;

  auto PruneExpiredTimer() noexcept -> std::vector<std::unique_ptr<SingleTimer>>;

 private:
  struct SingleTimerCompartor {
    auto operator()(const SingleTimer *lhs, const SingleTimer *rhs) const noexcept -> bool;
  };

  std::map<SingleTimer *, std::unique_ptr<SingleTimer>, SingleTimerCompartor> timer_queue_;
};

}  // namespace TURTLE_SERVER

#endif  // SRC_INCLUDE_CORE_TIMER_H_
