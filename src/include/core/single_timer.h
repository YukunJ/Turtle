/**
 * @file single_timer.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 11 2023
 *
 * This is a header file implementing the a single timer
 * that's associated with an expiring time and callback func
 */

#ifndef SRC_INCLUDE_CORE_SINGLE_TIMER_H_
#define SRC_INCLUDE_CORE_SINGLE_TIMER_H_

#include <cstdint>
#include <functional>

namespace TURTLE_SERVER {

/*
 * helper to get current time in milliseconds since epoch
 */
auto NowSinceEpoch() noexcept -> uint64_t;

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

}  // namespace TURTLE_SERVER

#endif  // SRC_INCLUDE_CORE_SINGLE_TIMER_H_
