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

#ifndef SRC_INCLUDE_LOOPER_H_
#define SRC_INCLUDE_LOOPER_H_

#include <atomic>
#include <functional>
#include <future>  // NOLINT
#include <map>
#include <memory>
#include <mutex>  // NOLINT

#include "acceptor.h"
#include "connection.h"
#include "poller.h"
#include "thread_pool.h"
#include "utils.h"


namespace TURTLE_SERVER {

/* the epoll_wait time in milliseconds */
static constexpr int TIMEOUT = 3000;

class Acceptor;
/**
 * This Looper acts as the central coordinator between executor (ThreadPool) and
 * event polling (Poller)
 * */
class Looper {
 public:
  explicit Looper(ThreadPool *pool);

  ~Looper() = default;

  NON_COPYABLE(Looper);

  void Loop();

  void AddAcceptor(std::unique_ptr<Acceptor> acceptor);

  void AddConnection(std::unique_ptr<Connection> new_conn);

  auto DeleteConnection(int fd) -> bool;

  auto DispatchTask(const std::function<void()> &task) -> std::future<void>;

  auto GetAcceptor() -> Acceptor *;

  void SetExit() noexcept;

 private:
  ThreadPool *pool_;
  std::unique_ptr<Poller> poller_;
  std::mutex mtx_;
  std::unique_ptr<Acceptor> acceptor_{nullptr};
  std::map<int, std::unique_ptr<Connection>> connections_;
  std::atomic<bool> exit_{false};
};
}  // namespace TURTLE_SERVER
#endif  // SRC_INCLUDE_LOOPER_H_
