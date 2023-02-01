/**
 * @file buffer_test.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 28 2023
 *
 * This is the unit test file for core/Buffer class
 */

#include "core/buffer.h"

#include <cstring>

#include "catch2/catch_test_macros.hpp"

/* for convenience reason */
using TURTLE_SERVER::Buffer;
using TURTLE_SERVER::INITIAL_BUFFER_CAPACITY;

TEST_CASE("[core/buffer]") {
  Buffer buf(INITIAL_BUFFER_CAPACITY);
  REQUIRE(buf.Size() == 0);
  REQUIRE(buf.Capacity() == INITIAL_BUFFER_CAPACITY);

  SECTION("appending c-str into buffer from both side") {
    const char msg1[1024] = "Greeting from beginning!";
    const char msg2[2014] = "Farewell from end~";
    buf.AppendHead(msg1);
    buf.Append(msg2);
    snprintf((char *)msg1 + strlen(msg1), strlen(msg2) + 1, "%s", msg2);
    CHECK(std::strncmp((char *)buf.Data(), msg1, std::strlen(msg1)) == 0);
    buf.Clear();
    CHECK(buf.Size() == 0);
  }

  SECTION("appending std::string into buffer from both side") {
    const std::string msg1 = "Greeting from beginning!";
    const std::string msg2 = "Farewell from end~";
    buf.AppendHead(msg1);
    buf.Append(msg2);
    const std::string together = msg1 + msg2;
    CHECK(buf.ToStringView() == together);
    buf.Clear();
    CHECK(buf.Size() == 0);
  }

  SECTION("find and pop based on the target first found") {
    const std::string msg =
        "GET / HTTP/1.1\r\n"
        "Connection: Keep-Alive\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
        "Accept-Language: en-us\r\n"
        "\r\n";
    const std::string next_msg = "Something belongs to next message";
    buf.Append(msg);
    buf.Append(next_msg);
    auto op_str = buf.FindAndPopTill("\r\n\r\n");
    CHECK((op_str.has_value() && op_str.value() == msg));
    CHECK(buf.ToStringView() == next_msg);
  }
}
