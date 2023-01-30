/**
 * @file socket_test.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 29 2023
 *
 * This is the unit test file for core/Socket class
 */
#include "core/socket.h"

#include <fcntl.h>

#include <thread>  // NOLINT

#include "catch2/catch_test_macros.hpp"
#include "core/net_address.h"

/* for convenience reason */
using TURTLE_SERVER::NetAddress;
using TURTLE_SERVER::Socket;

TEST_CASE("[core/socket]") {
  NetAddress local_host("127.0.0.1", 20080);
  Socket server_sock;

  // build the server socket
  server_sock.Bind(local_host);
  REQUIRE(server_sock.GetFd() != -1);
  server_sock.Listen();

  SECTION("move operations will switch fd") {
    int orig_fd = server_sock.GetFd();
    Socket another_sock(123);
    server_sock = std::move(another_sock);
    CHECK(server_sock.GetFd() == 123);
    CHECK(another_sock.GetFd() == orig_fd);

    Socket second_sock = std::move(another_sock);
    CHECK(second_sock.GetFd() == orig_fd);
    CHECK(another_sock.GetFd() == -1);
  }

  SECTION("non-blocking mode setting for socket") {
    Socket sock;
    sock.Bind(local_host);
    CHECK((sock.GetAttrs() & O_NONBLOCK) == 0);
    sock.SetNonBlocking();
    CHECK((sock.GetAttrs() & O_NONBLOCK) != 0);
  }

  SECTION("interaction between server and client socket") {
    NetAddress client_address;
    std::thread client_thread = std::thread([&]() {
      Socket client_sock;
      client_sock.Connect(local_host);
    });
    client_thread.detach();
    CHECK(server_sock.Accept(client_address) != -1);
  }
}
