/**
 * @file acceptor.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 27 2022
 *
 * This is a header file implementing the Acceptor that accepts all the incoming
 * new client connections and set up customer handle functions for new clients
 */

#ifndef SRC_INCLUDE_ACCEPTOR_H_
#define SRC_INCLUDE_ACCEPTOR_H_

#include <functional>
#include <memory>

#include "connection.h"
#include "looper.h"
#include "net_address.h"
#include "utils.h"

namespace TURTLE_SERVER {

/**
 * This Acceptor comes with basic functionality for accepting new client
 * connections and add its into the Poller More custom handling could be added
 * as well
 * */
class Acceptor {
 public:
  explicit Acceptor(Looper *looper, NetAddress server_address);

  ~Acceptor() = default;

  NON_COPYABLE(Acceptor);

  void BaseAcceptCallback(Connection *server_conn);

  void SetCustomAcceptCallback(
      std::function<void(Connection *)> custom_accept_callback);

  void SetCustomHandleCallback(
      std::function<void(Connection *)> custom_handle_callback);

  auto GetCustomAcceptCallback() -> std::function<void(Connection *)>;

  auto GetCustomHandleCallback() -> std::function<void(Connection *)>;

  auto GetAcceptorConnection() -> Connection *;

 private:
  Looper *looper_;
  std::unique_ptr<Connection> acceptor_conn;
  std::function<void(Connection *)> custom_accept_callback_{};
  std::function<void(Connection *)> custom_handle_callback_{};
};

}  // namespace TURTLE_SERVER

#endif  // SRC_INCLUDE_ACCEPTOR_H_
