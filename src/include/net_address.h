/**
 * @file net_address.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 25 2022
 *
 * This is a header file implementing the Internet Address, which is composed
 * of an IP address and a port
 */

#ifndef SRC_INCLUDE_NET_ADDRESS_H_
#define SRC_INCLUDE_NET_ADDRESS_H_

#include <arpa/inet.h>

#include <cstring>
#include <iostream>
#include <string>

namespace TURTLE_SERVER {

/* Network protocol supported */
enum class Protocol { Ipv4, Ipv6 };

/**
 * This NetAddress class encapsulates the unique identifier of a network host
 * in the form of "IP Address + Port"
 * This class is compatible with both IPv4 and IPv6
 * */
class NetAddress {
 public:
  NetAddress() noexcept;

  NetAddress(const char *ip, uint16_t port, Protocol protocol = Protocol::Ipv4);

  ~NetAddress() = default;

  auto GetProtocol() const noexcept -> Protocol;

  auto YieldAddr() -> struct sockaddr *;

  auto YieldAddrLen() -> socklen_t *;

  auto GetIp() const noexcept -> std::string;

  auto GetPort() const noexcept -> uint16_t;

  auto ToString() const noexcept -> std::string;

 private:
  const Protocol protocol_{Protocol::Ipv4};
  mutable struct sockaddr addr_ {};
  socklen_t addr_len_;
};

std::ostream &operator<<(std::ostream &os, const NetAddress &address);

}  // namespace TURTLE_SERVER
#endif  // SRC_INCLUDE_NET_ADDRESS_H_
