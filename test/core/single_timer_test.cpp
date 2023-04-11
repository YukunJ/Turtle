/**
 * @file single_timer_test.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 30 2023
 *
 * This is the unit test file for core/SingleTimer class
 */

#include "core/single_timer.h"

#include "catch2/catch_test_macros.hpp"

#include <chrono>  // NOLINT
#include <thread>  // NOLINT

TEST_CASE("[core/single_timer]") {
  uint64_t expire = 500;  // expire after 0.5 second
  SECTION("single timer expire accuracy") {
    TURTLE_SERVER::SingleTimer t{expire, nullptr};
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint64_t>(0.9 * expire)));
    REQUIRE(!t.Expired());
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint64_t>(0.2 * expire)));
    REQUIRE(t.Expired());
  }

  SECTION("single timer's callback execution") {
    int i = 0;
    TURTLE_SERVER::SingleTimer t{expire, [&]() { i++; }};
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint64_t>(1.1 * expire)));
    if (t.Expired()) {
      t.Run();
    }
    REQUIRE(t.Expired());
    REQUIRE(i == 1);
  }
}
