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

#ifndef SRC_INCLUDE_SOCKET_H_
#define SRC_INCLUDE_SOCKET_H_

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>

#include "net_address.h"
#include "utils.h"

namespace TURTLE_SERVER {

/**
 * This Socket class encapsulates a socket descriptor
 * which can act as either listener or client
 * This class is compatible with both IPv4 and IPv6
 * */
class Socket {
 public:
  explicit Socket(bool is_ipv4 = true);

  explicit Socket(int fd);

  NON_COPYABLE(Socket);

  Socket(Socket &&other) noexcept;

  Socket &operator=(Socket &&other) noexcept;

  ~Socket();

  auto GetFd() const -> int;

  /* for client, one step: directly connect */
  void Connect(NetAddress &server_address);  // NOLINT

  /* for server, three steps: bind + listen + accept */
  void Bind(NetAddress &server_address);  // NOLINT

  void Listen();

  auto Accept(NetAddress &client_address) -> int;  // NOLINT

  void SetReusable();

  void SetNonBlocking();

 private:
  int fd_{-1};
};
}  // namespace TURTLE_SERVER
#endif  // SRC_INCLUDE_SOCKET_H_
