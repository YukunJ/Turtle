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
namespace TURTLE_SERVER {

Looper::Looper() : poller_(std::make_unique<Poller>()) {}

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
}

auto Looper::DeleteConnection(int fd) -> bool {
  std::unique_lock<std::mutex> lock(mtx_);
  auto it = connections_.find(fd);
  if (it == connections_.end()) {
    return false;
  }
  connections_.erase(it);
  return true;
}

void Looper::SetExit() noexcept { exit_ = true; }

} // namespace TURTLE_SERVER
