/**
 * @file request_test.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Feb 1 2023
 *
 * This is the unit test file for http/Response class
 */

#include "http/response.h"

#include "catch2/catch_test_macros.hpp"
#include "http/header.h"
#include "http/http_utils.h"

/* for convenience reason */
using TURTLE_SERVER::HTTP::Header;
using TURTLE_SERVER::HTTP::HEADER_CONTENT_LENGTH;
using TURTLE_SERVER::HTTP::Response;
using TURTLE_SERVER::HTTP::RESPONSE_OK;

TEST_CASE("[http/response]") {
  SECTION("response should be able to modify header on the fly") {
    std::string status = "200 Success";
    Response response{RESPONSE_OK, false, std::string("nonexistent-file.txt")};
    auto headers = response.GetHeaders();
    bool find = false;
    for (auto& h : headers) {
      if (h.GetKey() == HEADER_CONTENT_LENGTH) {
        find = true;
      }
    }
    CHECK(find);
    std::string new_val = "1024";
    CHECK(response.ChangeHeader(HEADER_CONTENT_LENGTH, new_val));
    find = false;
    std::string value;
    headers = response.GetHeaders();
    for (auto& h : headers) {
      if (h.GetKey() == HEADER_CONTENT_LENGTH) {
        find = true;
        value = h.GetValue();
      }
    }
    CHECK(find);
    CHECK(value == new_val);
  }
}