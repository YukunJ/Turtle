/**
 * @file looper.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 26 2022
 *
 * This is a header file implementing the Looper that servers as the main
 * control looping for the server
 */

#ifndef TURTLE_SERVER_LOOPER_H
#define TURTLE_SERVER_LOOPER_H

#include <atomic>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>

#include "connection.h"
#include "poller.h"
#include "thread_pool.h"
#include "utils.h"
/* the epoll_wait time in milliseconds */
#define TIMEOUT 3000

namespace TURTLE_SERVER {

/**
 * This Looper acts as the central coordinator between executor (ThreadPool) and
 * event polling (Poller)
 * */
class Looper {
 public:
  explicit Looper(ThreadPool* pool);

  ~Looper() = default;

  NON_COPYABLE(Looper);

  void Loop();

  void AddConnection(std::unique_ptr<Connection> new_conn);

  auto DeleteConnection(int fd) -> bool;

  auto DispatchTask(const std::function<void()>& task) -> std::future<void>;

  void Exit();

 private:
  ThreadPool* pool_;
  std::unique_ptr<Poller> poller_;
  std::mutex mtx_;
  std::map<int, std::unique_ptr<Connection>> connections_;
  std::atomic<bool> exit_{false};
};
}  // namespace TURTLE_SERVER
#endif  // TURTLE_SERVER_LOOPER_H
