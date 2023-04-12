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

#include <chrono>  // NOLINT
#include <thread>  // NOLINT

TEST_CASE("[core/single_timer]") {
  uint64_t expire = 500;  // expire after 0.5 second
  SECTION("single timer expire accuracy") {
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
}
