/**
 * @file turtle_server.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 27 2022
 *
 * This is a header-file-only base class for the Turtle web server setup
 */
#include <memory>
#include <utility>

#include "acceptor.h"
#include "connection.h"
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
 * The base class for setting up a web server using the Turtle framework
 * User should design a class that inherits from the TurtleServer base class
 * and implements the two virtual function 'OnHandle' and 'OnAccept'
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
 * OnHandle(): No base version exists. Users should implement this function to
 * achieve the expected behavior
 */
class TurtleServer {
 public:
  explicit TurtleServer(NetAddress server_address)
      : pool_(std::make_unique<ThreadPool>()),
        looper_(std::make_unique<Looper>(pool_.get())) {
    auto acceptor = std::make_unique<Acceptor>(looper_.get(), server_address);
    acceptor->SetCustomHandleCallback(
        [this](auto &&PH1) { OnHandle(std::forward<decltype(PH1)>(PH1)); });
    acceptor->SetCustomAcceptCallback(
        [this](auto &&PH1) { OnAccept(std::forward<decltype(PH1)>(PH1)); });
    looper_->AddAcceptor(std::move(acceptor));
  }

  virtual ~TurtleServer() = default;

  /* Not Edge trigger */
  virtual void OnAccept(Connection *acceptor_conn) = 0;

  /* Edge trigger! Read all bytes please */
  virtual void OnHandle(Connection *client_conn) = 0;

  virtual void Begin() { looper_->Loop(); }

  auto GetPool() noexcept -> ThreadPool * { return pool_.get(); }
  auto GetLooper() noexcept -> Looper * { return looper_.get(); }

 private:
  std::unique_ptr<ThreadPool> pool_;
  std::unique_ptr<Looper> looper_;
};
}  // namespace TURTLE_SERVER

#endif  // SRC_INCLUDE_TURTLE_SERVER_H_
