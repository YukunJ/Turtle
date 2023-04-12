/**
 * @file timer.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 11 2023
 *
 * This is an implementation file implementing the Timer and SingleTimer class
 * which is associated with an expiring time and callback func
 */

#include "core/timer.h"

#include <chrono>  // NOLINT

namespace TURTLE_SERVER {

auto NowSinceEpoch() noexcept -> uint64_t {
  auto now = std::chrono::high_resolution_clock::now();
  auto duration = now.time_since_epoch();
  return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}

/* ---------- SingleTimer ------------ */
Timer::SingleTimer::SingleTimer(uint64_t expire_from_now, std::function<void()> callback) noexcept
    : expire_time_(NowSinceEpoch() + expire_from_now), callback_(std::move(callback)) {}

auto Timer::SingleTimer::WhenExpire() const noexcept -> uint64_t { return expire_time_; }

auto Timer::SingleTimer::Expired() const noexcept -> bool { return NowSinceEpoch() >= expire_time_; }

void Timer::SingleTimer::Run() noexcept {
  if (callback_) {
    callback_();
  }
}

/* ------------ Timer --------------- */
auto Timer::AddSingleTimer(uint64_t expire_from_now, const std::function<void()> &callback) noexcept
    -> Timer::SingleTimer * {
  auto new_timer = std::make_unique<SingleTimer>(expire_from_now, callback);
  auto raw_timer = new_timer.get();
  timer_queue_.emplace(raw_timer, std::move(new_timer));
  // timer_queue_.insert(std::pair<SingleTimer*, std::unique_ptr<SingleTimer>>(raw_timer, std::move(new_timer)));
  {
    // TODO: check if earliest expiration time has changed
  }
  return raw_timer;
}

auto Timer::RemoveSingleTimer(Timer::SingleTimer *single_timer) noexcept -> bool {
  auto it = timer_queue_.find(single_timer);
  if (it != timer_queue_.end()) {
    timer_queue_.erase(it);
    // TODO: check if earliest expiration time has changed
    return true;
  }
  return false;
}
auto Timer::NextExpireTime() const noexcept -> uint64_t {
   if (timer_queue_.empty()) {
       return 0;
   }
   return timer_queue_.begin()->first->WhenExpire();
}

auto Timer::TimerCount() const noexcept -> size_t { return timer_queue_.size(); }

auto Timer::PruneExpiredTimer() noexcept -> std::vector<std::unique_ptr<SingleTimer>> {
  std::vector<std::unique_ptr<SingleTimer>> expired;
  auto it = timer_queue_.begin();
  for (; it != timer_queue_.end(); it++) {
    if (!it->first->Expired()) {
      break;
    }
  }
  // begin till it has expired and we are to remove them
  for (auto expire_it = timer_queue_.begin(); expire_it != it; expire_it++) {
    expired.push_back(std::move(expire_it->second));
  }
  timer_queue_.erase(timer_queue_.begin(), it);
  return expired;
}

auto Timer::SingleTimerCompartor::operator()(const SingleTimer *lhs, const SingleTimer *rhs) const noexcept -> bool {
  return lhs->WhenExpire() < rhs->WhenExpire();
}

}  // namespace TURTLE_SERVER
