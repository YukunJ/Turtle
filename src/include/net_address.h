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

#ifndef TURTLE_SERVER_NET_ADDRESS_H
#define TURTLE_SERVER_NET_ADDRESS_H

#include <arpa/inet.h>

#include <cstring>
#include <iostream>
#include <string>

namespace TURTLE_SERVER {

/**
 * This NetAddress class encapsulates the unique identifier of a network host
 * in the form of "IP Address + Port"
 * This class is compatible with both IPv4 and IPv6
 * */
class NetAddress {
 public:
  explicit NetAddress(bool is_ipv4 = true);

  NetAddress(const char *ip, uint16_t port, bool is_ipv4 = true);

  ~NetAddress() = default;

  auto IsIpv4() const -> bool { return is_ipv4_; }

  auto YieldAddr() -> struct sockaddr * { return &addr_; };

  auto YieldAddrLen() -> socklen_t * { return &addr_len_; };

  auto GetIp() const -> std::string;

  auto GetPort() const -> uint16_t;

  auto ToString() const -> std::string;

  friend std::ostream &operator<<(std::ostream &os, const NetAddress &address);

 private:
  const bool is_ipv4_;
  mutable struct sockaddr addr_ {};
  socklen_t addr_len_;
};

}  // namespace TURTLE_SERVER
#endif  // TURTLE_SERVER_NET_ADDRESS_H