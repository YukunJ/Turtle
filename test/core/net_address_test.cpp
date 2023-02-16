/**
 * @file net_address_test.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 28 2023
 *
 * This is the unit test file for core/NetAddress class
 */

#include "core/net_address.h"

#include "catch2/catch_test_macros.hpp"

/* for convenience reason */
using TURTLE_SERVER::NetAddress;
using TURTLE_SERVER::Protocol;

TEST_CASE("[core/net_address]") {
  SECTION("Ipv4 net address") {
    NetAddress address_ipv4("127.0.0.1", 20080, Protocol::Ipv4);
    REQUIRE(address_ipv4.GetProtocol() == Protocol::Ipv4);
    REQUIRE(address_ipv4.GetIp() == std::string("127.0.0.1"));
    REQUIRE(address_ipv4.GetPort() == 20080);
    REQUIRE(address_ipv4.ToString() == std::string("127.0.0.1 @ 20080"));
  }

  SECTION("Ipv6 net address") {
    NetAddress address_ipv6("2001:db8:3333:4444:5555:6666:7777:8888", 40080, Protocol::Ipv6);
    REQUIRE(address_ipv6.GetProtocol() == Protocol::Ipv6);
    REQUIRE(address_ipv6.GetIp() == std::string("2001:db8:3333:4444:5555:6666:7777:8888"));
    REQUIRE(address_ipv6.GetPort() == 40080);
    REQUIRE(address_ipv6.ToString() == std::string("2001:db8:3333:4444:5555:6666:7777:8888 @ 40080"));
  }
}
