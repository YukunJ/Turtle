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
#include <cstring>
#include "core/connection.h"
#include "core/poller.h"
#include "core/socket.h"
#include "log/logger.h"

namespace TURTLE_SERVER {

static constexpr int MILLS_IN_SECOND = 1000;
static constexpr int NANOS_IN_MILL = 1000 * 1000;

auto NowSinceEpoch() noexcept -> uint64_t {
  auto now = std::chrono::high_resolution_clock::now();
  auto duration = now.time_since_epoch();
  return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}

auto FromNow(uint64_t timestamp) noexcept -> uint64_t {
  auto now = NowSinceEpoch();
  return (timestamp >= now) ? (timestamp - now) : 0;
}

auto FromNowInTimeSpec(uint64_t timestamp) noexcept -> struct timespec {
  auto from_now_mills = FromNow(timestamp);
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(from_now_mills / MILLS_IN_SECOND);
  ts.tv_nsec = static_cast<int64_t>((from_now_mills % MILLS_IN_SECOND) * NANOS_IN_MILL);
  return ts;
};

void ResetTimerFd(int timer_fd, struct timespec ts) {
  struct itimerspec new_value;
  struct itimerspec old_value;
  memset(&new_value, 0, sizeof(new_value));
  memset(&old_value, 0, sizeof(old_value));
  new_value.it_value = ts;
  int ret = timerfd_settime(timer_fd, 0, &new_value, &old_value);
  if (ret < 0) {
    LOG_ERROR("ResetTimerFd(): timerfd_settime fails");
  }
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
Timer::Timer() : timer_fd_(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)) {
  if (timer_fd_ < 0) {
    LOG_FATAL("Timer(): timerfd_create fails");
    exit(EXIT_FAILURE);
  }
  timer_conn_ = std::make_unique<Connection>(std::make_unique<Socket>(timer_fd_));
  timer_conn_->SetEvents(POLL_READ | POLL_ET);  // enable READ in Epoll
}

auto Timer::GetTimerConnection() -> Connection * { return timer_conn_.get(); }

auto Timer::GetTimerFd() -> int { return timer_fd_; }

auto Timer::AddSingleTimer(uint64_t expire_from_now, const std::function<void()> &callback) noexcept
    -> Timer::SingleTimer * {
  auto new_timer = std::make_unique<SingleTimer>(expire_from_now, callback);
  auto raw_timer = new_timer.get();
  timer_queue_.emplace(raw_timer, std::move(new_timer));
  uint64_t new_next_expire = NextExpireTime();
  if (new_next_expire != next_expire_) {
    next_expire_ = new_next_expire;
    ResetTimerFd(timer_fd_, FromNowInTimeSpec(new_next_expire));
  }
  return raw_timer;
}

auto Timer::RemoveSingleTimer(Timer::SingleTimer *single_timer) noexcept -> bool {
  auto it = timer_queue_.find(single_timer);
  if (it != timer_queue_.end()) {
    timer_queue_.erase(it);
    uint64_t new_next_expire = NextExpireTime();
    if (new_next_expire != next_expire_) {
      next_expire_ = new_next_expire;
      ResetTimerFd(timer_fd_, FromNowInTimeSpec(new_next_expire));
    }
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
  uint64_t new_next_expire = NextExpireTime();
  if (new_next_expire != next_expire_) {
    next_expire_ = new_next_expire;
    ResetTimerFd(timer_fd_, FromNowInTimeSpec(new_next_expire));
  }
  return expired;
}

auto Timer::SingleTimerCompartor::operator()(const SingleTimer *lhs, const SingleTimer *rhs) const noexcept -> bool {
  return lhs->WhenExpire() < rhs->WhenExpire();
}

}  // namespace TURTLE_SERVER
