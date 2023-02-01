/**
 * @file acceptor_test.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 31 2023
 *
 * This is the unit test file for core/Acceptor class
 */
#include "core/acceptor.h"

#include <unistd.h>

#include <future>  // NOLINT
#include <memory>
#include <vector>

#include "catch2/catch_test_macros.hpp"
#include "core/connection.h"
#include "core/looper.h"
#include "core/net_address.h"
#include "core/poller.h"
#include "core/socket.h"
#include "core/thread_pool.h"

/* for convenience reason */
using TURTLE_SERVER::Acceptor;
using TURTLE_SERVER::Connection;
using TURTLE_SERVER::Looper;
using TURTLE_SERVER::NetAddress;
using TURTLE_SERVER::POLL_ADD;
using TURTLE_SERVER::POLL_ET;
using TURTLE_SERVER::POLL_READ;
using TURTLE_SERVER::Poller;
using TURTLE_SERVER::Socket;
using TURTLE_SERVER::ThreadPool;

TEST_CASE("[core/acceptor]") {
  NetAddress local_host("127.0.0.1", 20080);

  ThreadPool pool;

  // built an acceptor will one listener looper and one reactor together
  auto single_reactor = std::make_unique<Looper>();

  std::vector<Looper *> raw_reactors = {single_reactor.get()};
  auto acceptor = Acceptor(single_reactor.get(), raw_reactors, local_host);

  REQUIRE(acceptor.GetAcceptorConnection()->GetFd() != -1);

  SECTION(
      "Acceptor should be able to accept new clients and set callback for "
      "them") {
    int client_num = 3;
    std::atomic<int> accept_trigger = 0;
    std::atomic<int> handle_trigger = 0;

    // set acceptor customize functions
    acceptor.SetCustomAcceptCallback([&](Connection *) { accept_trigger++; });
    acceptor.SetCustomHandleCallback([&](Connection *) { handle_trigger++; });

    // start three clients and connect with server
    const char *msg = "Hello from client!";
    std::vector<std::future<void>> futs;
    for (int i = 0; i < client_num; i++) {
      auto fut = std::async(std::launch::async, [&]() {
        Socket client_sock;
        client_sock.Connect(local_host);
        CHECK(client_sock.GetFd() != -1);
        send(client_sock.GetFd(), msg, strlen(msg), 0);
      });
      futs.push_back(std::move(fut));
    }

    auto runner =
        std::async(std::launch::async, [&]() { single_reactor->Loop(); });
    futs.push_back(std::move(runner));
    sleep(2);
    single_reactor->SetExit();  // terminate the looper

    // accept & handle should be triggered exactly 3 times
    CHECK(accept_trigger == client_num);
    CHECK(handle_trigger == client_num);

    for (auto &f : futs) {
      f.wait();
    }
  }
}
