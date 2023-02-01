/**
 * @file cache_test.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 31 2023
 *
 * This is the unit test file for core/Cache class
 */

#include "core/cache.h"

#include <vector>

#include "catch2/catch_test_macros.hpp"

/* for convenience reason */
using TURTLE_SERVER::Cache;

TEST_CASE("[core/cache]") {
  const int capacity = 20;
  Cache cache(capacity);
  std::vector<unsigned char> data = {'h', 'e', 'l', 'l', 'o', '!'};
  const auto data_size = data.size();

  REQUIRE(cache.GetOccupancy() == 0);
  REQUIRE(cache.GetCapacity() == capacity);

  SECTION(
      "cache should be able to cache data up to capacity, and start evict") {
    for (int i = 1; i <= capacity / data_size; i++) {
      std::string url = "url" + std::to_string(i);
      bool cache_success = cache.TryInsert(url, data);
      CHECK(cache_success);
      CHECK(cache.GetOccupancy() == i * data_size);
    }

    std::vector<unsigned char> read_buf;
    // all url1, url2, url3 should be available
    for (int i = 1; i <= capacity / data_size; i++) {
      auto load_success = cache.TryLoad("url" + std::to_string(i), read_buf);
      CHECK(load_success);
    }
    CHECK(read_buf.size() == ((capacity / data_size) * data_size));

    // now is 3 * 6 = 18 bytes, next insert should evict the first guy
    bool cache_success = cache.TryInsert("url4", data);
    CHECK(cache_success);

    // url1 should be evicted
    bool load_success = cache.TryLoad("url1", read_buf);
    CHECK(!load_success);
  }
}
