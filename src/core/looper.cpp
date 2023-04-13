/**
 * @file looper.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 26 2022
 *
 * This is an implementation file implementing the Looper that servers as the
 * main control looping for the server
 */

#include "core/looper.h"

#include "core/acceptor.h"
#include "core/connection.h"
#include "core/poller.h"
#include "core/thread_pool.h"
#include "log/logger.h"
namespace TURTLE_SERVER {

Looper::Looper(bool use_timer) : poller_(std::make_unique<Poller>()), use_timer_(use_timer) {
  if (use_timer_) {
    poller_->AddConnection(timer_.GetTimerConnection());
  }
}

void Looper::Loop() {
  while (!exit_) {
    auto ready_connections = poller_->Poll(TIMEOUT);
    for (auto &conn : ready_connections) {
      conn->GetCallback()();
    }
  }
}

void Looper::AddAcceptor(Connection *acceptor_conn) {
  std::unique_lock<std::mutex> lock(mtx_);
  poller_->AddConnection(acceptor_conn);
}

void Looper::AddConnection(std::unique_ptr<Connection> new_conn) {
  std::unique_lock<std::mutex> lock(mtx_);
  poller_->AddConnection(new_conn.get());
  int fd = new_conn->GetFd();
  connections_.insert({fd, std::move(new_conn)});
  if (use_timer_) {
    auto single_timer = timer_.AddSingleTimer(INACTIVE_TIMEOUT, [this, fd = fd]() {
      LOG_INFO("client fd=" + std::to_string(fd) + " has expired and will be kicked out");
      DeleteConnection(fd);
    });
    timers_mapping_.insert({fd, single_timer});
  }
}

auto Looper::DeleteConnection(int fd) -> bool {
  std::unique_lock<std::mutex> lock(mtx_);
  auto it = connections_.find(fd);
  if (it == connections_.end()) {
    return false;
  }
  connections_.erase(it);
  if (use_timer_) {
    auto timer_it = timers_mapping_.find(fd);
    if (timer_it != timers_mapping_.end()) {
      timer_.RemoveSingleTimer(timer_it->second);
    } else {
      LOG_ERROR("Looper: DeleteConnection() the fd " + std::to_string(fd) + " not in timers_mapping_");
    }
  }
  return true;
}

void Looper::SetExit() noexcept { exit_ = true; }

}  // namespace TURTLE_SERVER
