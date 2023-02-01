/**
 * @file cgier_test.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Feb 1 2023
 *
 * This is the unit test file for http/Cgier class
 */

#include "http/cgier.h"

#include "catch2/catch_test_macros.hpp"
#include "http/http_utils.h"

/* for convenience reason */
using TURTLE_SERVER::HTTP::Cgier;
using TURTLE_SERVER::HTTP::IsFileExists;

TEST_CASE("[http/response]") {
  // run two sample cgi program to test result

  SECTION("cgi add program") {
    std::string program = "./add";
    CHECK(IsFileExists(program));
    int a = 1;
    int b = 2;
    int c = a + b;
    std::string expected_str = "cgi program add(" + std::to_string(a) + ", " +
                               std::to_string(b) + ") = " + std::to_string(c) +
                               "\n";
    Cgier cgier(program, {std::to_string(a), std::to_string(b)});
    auto result = cgier.Run();
    std::string ret_str = std::string(result.begin(), result.end());

    CHECK(ret_str == expected_str);
  }

  SECTION("cgi helloworld program") {
    std::string program = "./helloworld";
    std::string expected_str = "Hello World from no argument cgi program\n";
    CHECK(IsFileExists(program));
    Cgier cgier(program, {});
    auto result = cgier.Run();
    std::string ret_str = std::string(result.begin(), result.end());

    CHECK(ret_str == expected_str);
  }
}
