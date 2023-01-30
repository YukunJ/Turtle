/**
 * @file poller_test.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 30 2023
 *
 * This is the unit test file for core/Poller class
 */

#include "core/poller.h"

#include <unistd.h>

#include <memory>
#include <thread>  // NOLINT
#include <vector>

#include "catch2/catch_test_macros.hpp"
#include "core/connection.h"
#include "core/net_address.h"
#include "core/socket.h"

/* for convenience reason */
using TURTLE_SERVER::Connection;
using TURTLE_SERVER::NetAddress;
using TURTLE_SERVER::POLL_ADD;
using TURTLE_SERVER::POLL_ET;
using TURTLE_SERVER::POLL_READ;
using TURTLE_SERVER::Poller;
using TURTLE_SERVER::Socket;

TEST_CASE("[core/poller]") {
  NetAddress local_host("127.0.0.1", 20080);
  Socket server_sock;

  // build the server socket
  server_sock.Bind(local_host);
  server_sock.Listen();
  REQUIRE(server_sock.GetFd() != -1);

  int client_num = 3;
  // build the empty poller
  Poller poller(client_num);
  REQUIRE(poller.GetPollSize() == client_num);

  SECTION("able to poll out the client's messages sent over") {
    std::vector<std::thread> threads;
    for (int i = 0; i < client_num; i++) {
      threads.emplace_back([&]() {
        auto client_socket = Socket();
        client_socket.Connect(local_host);
        char message[] = "Hello from client!";
        send(client_socket.GetFd(), message, strlen(message), 0);
      });
    }

    // server accept clients and build connection for them
    std::vector<std::shared_ptr<Connection>> client_conns;
    for (int i = 0; i < client_num; i++) {
      NetAddress client_address;
      auto client_sock =
          std::make_unique<Socket>(server_sock.Accept(client_address));
      CHECK(client_sock->GetFd() != -1);
      client_conns.push_back(
          std::make_shared<Connection>(std::move(client_sock)));
      client_conns[i]->SetEvents(POLL_READ);
    }

    // each client connection under poller's monitor
    for (int i = 0; i < client_num; i++) {
      poller.AddConnection(client_conns[i].get());
    }

    auto ready_conns = poller.Poll();
    CHECK(ready_conns.size() == client_num);

    for (int i = 0; i < client_num; i++) {
      threads[i].join();
    }
  }
}
