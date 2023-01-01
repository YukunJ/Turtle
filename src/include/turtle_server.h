/**
 * @file turtle_server.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 1 2023
 *
 * This is a header-file-only class for the Turtle web server setup
 */
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

#include "acceptor.h"
#include "connection.h"
#include "http/header.h"
#include "http/http_utils.h"
#include "http/request.h"
#include "http/response.h"
#include "looper.h"
#include "net_address.h"
#include "poller.h"
#include "socket.h"
#include "thread_pool.h"
#include "utils.h"

#ifndef SRC_INCLUDE_TURTLE_SERVER_H_
#define SRC_INCLUDE_TURTLE_SERVER_H_

namespace TURTLE_SERVER {

/**
 * The class for setting up a web server using the Turtle framework
 * User should provide the callback functions in OnAccept() and OnHandle()
 * The rest is already taken care of and in most cases users don't need to touch
 * upon
 *
 * OnAccept(): Given the acceptor connection, when the Poller tells us there is
 * new incoming client connection basic step of socket accept and build
 * connection and add into the Poller are already taken care of in the
 * Acceptor::BaseAcceptCallback. This OnAccept() functionality is appended to
 * that base BaseAcceptCallback and called after that base, to support any
 * custom business logic upon receiving new client connection
 *
 * OnHandle(): No base version exists. Users should implement provide a function
 * to achieve the expected behavior
 */
class TurtleServer {
 public:
  explicit TurtleServer(NetAddress server_address)
      : pool_(std::make_unique<ThreadPool>()),
        looper_(std::make_unique<Looper>(pool_.get())) {
    auto acceptor = std::make_unique<Acceptor>(looper_.get(), server_address);
    looper_->AddAcceptor(std::move(acceptor));
  }

  virtual ~TurtleServer() = default;

  /* Not Edge trigger */
  auto OnAccept(std::function<void(Connection *)> on_accept) -> TurtleServer & {
    looper_->GetAcceptor()->SetCustomAcceptCallback(std::move(on_accept));
    return *this;
  }

  /* Edge trigger! Read all bytes please */
  auto OnHandle(std::function<void(Connection *)> on_handle) -> TurtleServer & {
    looper_->GetAcceptor()->SetCustomHandleCallback(std::move(on_handle));
    on_handle_set_ = true;
    return *this;
  }

  void Begin() {
    if (!on_handle_set_) {
      throw std::logic_error(
          "Please specify OnHandle callback function before starts");
    }
    looper_->Loop();
  }

  auto GetLooper() noexcept -> Looper * { return looper_.get(); }

 private:
  bool on_handle_set_{false};
  std::unique_ptr<ThreadPool> pool_;
  std::unique_ptr<Looper> looper_;
};
}  // namespace TURTLE_SERVER

#endif  // SRC_INCLUDE_TURTLE_SERVER_H_
