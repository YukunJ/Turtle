/**
 * @file timer_test.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 30 2023
 *
 * This is the unit test file for core/Timer class
 */

#include "core/timer.h"
#include "catch2/catch_test_macros.hpp"
#include "core/acceptor.h"
#include "core/connection.h"
#include "core/looper.h"
#include "core/net_address.h"
#include "core/poller.h"

#include <chrono>  // NOLINT
#include <thread>  // NOLINT

TEST_CASE("[core/single_timer]") {
  uint64_t expire = 500;  // expire after 0.5 second
  SECTION("single timer expire accurately") {
    TURTLE_SERVER::Timer::SingleTimer t{expire, nullptr};
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint64_t>(0.9 * expire)));
    REQUIRE(!t.Expired());
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint64_t>(0.2 * expire)));
    REQUIRE(t.Expired());
  }

  SECTION("single timer's callback execution") {
    int i = 0;
    TURTLE_SERVER::Timer::SingleTimer t{expire, [&]() { i++; }};
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint64_t>(1.1 * expire)));
    if (t.Expired()) {
      t.Run();
    }
    REQUIRE(t.Expired());
    REQUIRE(i == 1);
  }
}

TEST_CASE("[core/timer]") {
  SECTION("timer queue is properly sorted by earliest expiration time") {
    TURTLE_SERVER::Timer t;
    REQUIRE(t.NextExpireTime() == 0);
    auto now = TURTLE_SERVER::NowSinceEpoch();
    t.AddSingleTimer(200, nullptr);
    t.AddSingleTimer(100, nullptr);
    t.AddSingleTimer(500, nullptr);
    auto next_expire = t.NextExpireTime();
    REQUIRE((next_expire < (now + 110) && next_expire > (now + 90)));
    REQUIRE(t.TimerCount() == 3);
  }

  SECTION("timer queue is able to refresh ane existing timer") {
    TURTLE_SERVER::Timer t;
    REQUIRE(t.NextExpireTime() == 0);
    auto now = TURTLE_SERVER::NowSinceEpoch();
    auto raw_timer = t.AddSingleTimer(200, nullptr);
    auto next_expire = t.NextExpireTime();
    REQUIRE((next_expire < (now + 210) && next_expire > (now + 190)));
    std::this_thread::sleep_for(std::chrono::milliseconds(200));  // should expired by now
    t.RefreshSingleTimer(raw_timer, 200);
    next_expire = t.NextExpireTime();
    REQUIRE((next_expire < (now + 410) && next_expire > (now + 390)));
  }

  SECTION("timer queue is able to remove a timer based on raw pointer") {
    TURTLE_SERVER::Timer t;
    t.AddSingleTimer(200, nullptr);
    auto expire_1 = t.NextExpireTime();
    auto raw_ptr = t.AddSingleTimer(100, nullptr);
    t.AddSingleTimer(500, nullptr);
    auto expire_2 = t.NextExpireTime();
    REQUIRE(expire_2 < expire_1);
    REQUIRE(t.RemoveSingleTimer(raw_ptr) == true);
    REQUIRE(t.TimerCount() == 2);
    REQUIRE(expire_1 == t.NextExpireTime());
  }

  SECTION("timer queue able to prune expired timer") {
    TURTLE_SERVER::Timer t;
    t.AddSingleTimer(400, nullptr);
    t.AddSingleTimer(100, nullptr);
    t.AddSingleTimer(500, nullptr);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    auto expired = t.PruneExpiredTimer();
    REQUIRE(expired.size() == 1);
    REQUIRE(t.TimerCount() == 2);
    auto now = TURTLE_SERVER::NowSinceEpoch();
    auto next_expire = t.NextExpireTime();
    REQUIRE((next_expire > (now + 150) && next_expire < (now + 250)));
  }

  SECTION("timer's timer_fd could work with Epoll") {
    TURTLE_SERVER::Poller poller;
    TURTLE_SERVER::Timer t;
    int timeout = 100;
    poller.AddConnection(t.GetTimerConnection());
    auto triggered = poller.Poll(timeout);
    REQUIRE(triggered.empty());
    t.AddSingleTimer(100, nullptr);
    t.AddSingleTimer(200, nullptr);
    t.AddSingleTimer(300, nullptr);
    std::this_thread::sleep_for(std::chrono::milliseconds(110));
    triggered = poller.Poll(timeout);
    REQUIRE(triggered.size() == 1);
    auto expired = t.PruneExpiredTimer();
    REQUIRE(expired.size() == 1);
    REQUIRE(t.TimerCount() == 2);
    auto now = TURTLE_SERVER::NowSinceEpoch();
    auto next_expire = t.NextExpireTime();
    REQUIRE((next_expire > now + 75 && next_expire < now + 125));
  }

  SECTION("timer is really triggered when running in Looper") {
    // this test relies on put a std::cout in the Looper
    // to check if a connection has timed out
    // there is no very clear way to acquire this knowledge
    // under current architecture
    // built an acceptor will one listener looper and one reactor together
    std::vector<std::thread> threads;
    TURTLE_SERVER::NetAddress local_host("127.0.0.1", 20080);
    auto single_reactor = std::make_unique<TURTLE_SERVER::Looper>(3000);

    std::vector<TURTLE_SERVER::Looper *> raw_reactors = {single_reactor.get()};
    auto acceptor = TURTLE_SERVER::Acceptor(single_reactor.get(), raw_reactors, local_host);
    REQUIRE(acceptor.GetAcceptorConnection()->GetFd() != -1);

    threads.emplace_back([&]() { single_reactor->Loop(); });
    threads.emplace_back([&]() {
      // create a client that stays inactive for more than 3 seconds to be expired
      TURTLE_SERVER::Socket client_sock;
      client_sock.Connect(local_host);
      CHECK(client_sock.GetFd() != -1);
      std::this_thread::sleep_for(std::chrono::milliseconds(4000));
      // check std output to verify killed
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    single_reactor->SetExit();
    for (auto &t : threads) {
      if (t.joinable()) {
        t.join();
      }
    }
  }
}
