/**
 * @file connection_test.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 29 2023
 *
 * This is the unit test file for core/Connection class
 */

#include "core/connection.h"

#include <unistd.h>
#include <cstring>
#include <memory>
#include <string>
#include <thread>  // NOLINT

#include "catch2/catch_test_macros.hpp"
#include "core/net_address.h"
#include "core/poller.h"
#include "core/socket.h"

/* for convenience reason */
using TURTLE_SERVER::Connection;
using TURTLE_SERVER::NetAddress;
using TURTLE_SERVER::POLL_ADD;
using TURTLE_SERVER::POLL_ET;
using TURTLE_SERVER::POLL_READ;
using TURTLE_SERVER::Socket;

TEST_CASE("[core/connection]") {
  NetAddress local_host("127.0.0.1", 20080);
  auto server_sock = std::make_unique<Socket>();
  server_sock->Bind(local_host);
  server_sock->Listen();
  Connection server_conn(std::move(server_sock));
  REQUIRE(server_conn.GetSocket() != nullptr);

  SECTION("connection set events and return events") {
    server_conn.SetEvents(POLL_ADD | POLL_ET);
    CHECK((server_conn.GetEvents() & POLL_ADD));
    CHECK((server_conn.GetEvents() & POLL_ET));
    server_conn.SetRevents(POLL_READ);
    CHECK((server_conn.GetRevents() & POLL_READ));
  }

  SECTION("connection's callback setup and invoke") {
    server_conn.SetCallback([](Connection *) -> void {});
    int i = 0;
    server_conn.SetCallback([&target = i](Connection *) -> void { target += 1; });
    server_conn.GetCallback()();
    CHECK(i == 1);
  }

  SECTION("through connection to send and recv messages") {
    const char *client_message = "hello from client";
    const char *server_message = "hello from server";
    std::thread client_thread([&]() {
      // build a client connecting with server
      auto client_sock = std::make_unique<Socket>();
      client_sock->Connect(local_host);
      Connection client_conn(std::move(client_sock));
      // send a message to server
      client_conn.WriteToWriteBuffer(client_message);
      CHECK(client_conn.GetWriteBufferSize() == strlen(client_message));
      client_conn.Send();
      // recv a message from server
      sleep(1);
      auto [read, exit] = client_conn.Recv();
      CHECK((read == strlen(server_message) && exit));
      CHECK(client_conn.ReadAsString() == std::string(server_message));
    });

    client_thread.detach();
    NetAddress client_address;
    auto connected_sock = std::make_unique<Socket>(server_conn.GetSocket()->Accept(client_address));
    connected_sock->SetNonBlocking();
    CHECK(connected_sock->GetFd() != -1);
    Connection connected_conn(std::move(connected_sock));
    sleep(1);
    // recv a message from client
    auto [read, exit] = connected_conn.Recv();
    CHECK((read == strlen(client_message) && !exit));
    CHECK(connected_conn.GetReadBufferSize() == strlen(client_message));
    // send a message to client
    connected_conn.WriteToWriteBuffer(server_message);
    connected_conn.Send();
    sleep(1);
  }
}
