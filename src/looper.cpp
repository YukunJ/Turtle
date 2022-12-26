/**
 * @file looper.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 26 2022
 *
 * This is an implementation file implementing the Looper that servers as the
 * main control looping for the server
 */

#include <looper.h>

namespace TURTLE_SERVER {

Looper::Looper(ThreadPool* pool)
    : poller_(std::make_unique<Poller>()), pool_(pool) {}

void Looper::Loop() {
  while (!exit_) {
    auto ready_connections = poller_->Poll(TIMEOUT);
    for (auto& conn : ready_connections) {
      auto fut = DispatchTask(conn->GetCallback());
    }
  }
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

auto Looper::DispatchTask(const std::function<void()>& task)
    -> std::future<void> {
  return pool_->SubmitTask(task);
}

void Looper::Exit() { exit_ = true; }

}  // namespace TURTLE_SERVER