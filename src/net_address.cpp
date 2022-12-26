/**
 * @file net_address.cpp
 * @author Yukun J
 * @expectation this file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 25 2022
 *
 * This is an implementation file implementing the Internet Address, which is
 * composed of an IP address and a port
 */

#include "net_address.h"

namespace TURTLE_SERVER {

NetAddress::NetAddress(bool is_ipv4) : is_ipv4_(is_ipv4) {
  memset(&addr_, 0, sizeof(addr_));
  addr_len_ = sizeof(addr_);
}

NetAddress::NetAddress(const char *ip, uint16_t port, bool is_ipv4)
    : is_ipv4_(is_ipv4) {
  memset(&addr_, 0, sizeof(addr_));
  addr_len_ = sizeof(addr_);
  if (is_ipv4) {
    auto addr_ipv4 = reinterpret_cast<struct sockaddr_in *>(&addr_);
    addr_ipv4->sin_family = AF_INET;
    inet_pton(AF_INET, ip, &addr_ipv4->sin_addr.s_addr);
    addr_ipv4->sin_port = htons(port);
  } else {
    auto addr_ipv6 = reinterpret_cast<struct sockaddr_in6 *>(&addr_);
    addr_ipv6->sin6_family = AF_INET6;
    inet_pton(AF_INET6, ip, &addr_ipv6->sin6_addr.s6_addr);
    addr_ipv6->sin6_port = htons(port);
  }
}

auto NetAddress::GetIp() const -> std::string {
  char ip_address[INET6_ADDRSTRLEN];  // long enough for both Ipv4 and Ipv6
  if (is_ipv4_) {
    auto addr_ipv4 = reinterpret_cast<struct sockaddr_in *>(&addr_);
    inet_ntop(AF_INET, &addr_ipv4->sin_addr, ip_address, INET_ADDRSTRLEN);
  } else {
    auto addr_ipv6 = reinterpret_cast<struct sockaddr_in6 *>(&addr_);
    inet_ntop(AF_INET6, &addr_ipv6->sin6_addr, ip_address, INET6_ADDRSTRLEN);
  }
  return ip_address;
}

auto NetAddress::GetPort() const -> uint16_t {
  uint16_t port;
  if (is_ipv4_) {
    auto addr_ipv4 = reinterpret_cast<struct sockaddr_in *>(&addr_);
    port = ntohs(addr_ipv4->sin_port);
  } else {
    auto addr_ipv6 = reinterpret_cast<struct sockaddr_in6 *>(&addr_);
    port = ntohs(addr_ipv6->sin6_port);
  }
  return port;
}

auto NetAddress::ToString() const -> std::string {
  return GetIp() + std::string(" @ ") + std::to_string(GetPort());
}

std::ostream &operator<<(std::ostream &os, const NetAddress &address) {
  os << address.ToString();
  return os;
}
}  // namespace TURTLE_SERVER