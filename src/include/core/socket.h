/**
 * @file socket.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 25 2022
 *
 * This is a header file implementing the Socket, which acts as either the
 * listener or client
 */

#ifndef SRC_INCLUDE_CORE_SOCKET_H_
#define SRC_INCLUDE_CORE_SOCKET_H_

#include "core/utils.h"

namespace TURTLE_SERVER {

class NetAddress;

enum class Protocol;

/**
 * This Socket class encapsulates a socket descriptor
 * which can act as either listener or client
 * This class is compatible with both IPv4 and IPv6
 * */
class Socket {
 public:
  Socket() noexcept = default;

  explicit Socket(int fd) noexcept;

  NON_COPYABLE(Socket);

  Socket(Socket &&other) noexcept;

  auto operator=(Socket &&other) noexcept -> Socket &;

  ~Socket();

  auto GetFd() const noexcept -> int;

  /* for client, one step: directly connect */
  void Connect(NetAddress &server_address);  // NOLINT

  /* for server, three steps: bind + listen + accept */
  void Bind(NetAddress &server_address, bool set_reusable = true);  // NOLINT

  void Listen();

  auto Accept(NetAddress &client_address) -> int;  // NOLINT

  void SetReusable();

  void SetNonBlocking();

  auto GetAttrs() -> int;

 private:
  void CreateByProtocol(Protocol protocol);

  int fd_{-1};
};
}  // namespace TURTLE_SERVER
#endif  // SRC_INCLUDE_CORE_SOCKET_H_
