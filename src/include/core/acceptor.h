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
#ifndef SRC_INCLUDE_CORE_ACCEPTOR_H_
#define SRC_INCLUDE_CORE_ACCEPTOR_H_

#include <functional>
#include <memory>
#include <vector>

#include "core/utils.h"
namespace TURTLE_SERVER {

class NetAddress;
class Looper;
class Connection;

/**
 * This Acceptor comes with basic functionality for accepting new client
 * connections and distribute its into the different Poller.
 * More custom handling could be added as well
 * */
class Acceptor {
 public:
  Acceptor(Looper *listener, std::vector<Looper *> reactors,
                    NetAddress server_address);

  ~Acceptor() = default;

  NON_COPYABLE(Acceptor);

  void BaseAcceptCallback(Connection *server_conn);

  void SetCustomAcceptCallback(
      std::function<void(Connection *)> custom_accept_callback);

  void SetCustomHandleCallback(
      std::function<void(Connection *)> custom_handle_callback);

  auto GetCustomAcceptCallback() const noexcept
      -> std::function<void(Connection *)>;

  auto GetCustomHandleCallback() const noexcept
      -> std::function<void(Connection *)>;

  auto GetAcceptorConnection() noexcept -> Connection *;

 private:
  std::vector<Looper *> reactors_;
  Connection* acceptor_conn;
  std::function<void(Connection *)> custom_accept_callback_{};
  std::function<void(Connection *)> custom_handle_callback_{};
};

}  // namespace TURTLE_SERVER

#endif  // SRC_INCLUDE_CORE_ACCEPTOR_H_
