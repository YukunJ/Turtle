/**
 * @file thread_pool_test.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 29 2023
 *
 * This is the unit test file for core/ThreadPool class
 */

#include "core/thread_pool.h"

#include <atomic>

#include "catch2/catch_test_macros.hpp"

/* for convenience reason */
using TURTLE_SERVER::ThreadPool;

TEST_CASE("[core/thread_pool]") {
  static int thread_pool_size = 8;
  ThreadPool pool(thread_pool_size);
  REQUIRE(pool.GetSize() == thread_pool_size);

  SECTION("launch multiple tasks and wait for exit") {
    std::atomic<int> var = 0;
    {
      ThreadPool local_pool(thread_pool_size);
      for (int i = 0; i < 3 * thread_pool_size; i++) {
        local_pool.SubmitTask([&]() { var++; });
      }
      // here thread_pool's dtor should finish all the tasks
    }
    CHECK(var == 3 * thread_pool_size);
  }
}
