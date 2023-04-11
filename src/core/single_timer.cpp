/**
 * @file single_timer.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 11 2023
 *
 * This is an implementation file implementing the SingleTimer class
 * which is associated with an expiring time and callback func
 */

#include "core/single_timer.h"

#include <chrono>  // NOLINT

namespace TURTLE_SERVER {

auto NowSinceEpoch() noexcept -> uint64_t {
  auto now = std::chrono::high_resolution_clock::now();
  auto duration = now.time_since_epoch();
  return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}

SingleTimer::SingleTimer(uint64_t expire_from_now, std::function<void()> callback) noexcept
    : expire_time_(NowSinceEpoch() + expire_from_now), callback_(std::move(callback)) {}

auto SingleTimer::WhenExpire() const noexcept -> uint64_t { return expire_time_; }

auto SingleTimer::Expired() const noexcept -> bool { return NowSinceEpoch() >= expire_time_; }

void SingleTimer::Run() noexcept {
  if (callback_) {
    callback_();
  }
}
}  // namespace TURTLE_SERVER
