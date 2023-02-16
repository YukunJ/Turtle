/**
 * @file request_test.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Feb 1 2023
 *
 * This is the unit test file for http/Request class
 */

#include "http/request.h"

#include "catch2/catch_test_macros.hpp"
#include "http/header.h"
#include "http/http_utils.h"

/* for convenience reason */
using TURTLE_SERVER::HTTP::Method;
using TURTLE_SERVER::HTTP::Request;
using TURTLE_SERVER::HTTP::Version;

TEST_CASE("[http/request]") {
  SECTION("valid & invalid parsing of a request from string") {
    /* not end with /r/n */
    std::string request_0_str =
        "GET /hello.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/4.0\r\n"
        "Host: www.tutorialspoint.com\r\n"
        "Connection: Keep-Alive\r\n";
    Request request_0{request_0_str};
    CHECK(!request_0.IsValid());

    /* first line is not request line */
    std::string request_1_str =
        "User-Agent: Mozilla/4.0\r\n"
        "GET /hello.html HTTP/1.1\r\n"
        "Host: www.tutorialspoint.com\r\n"
        "Connection: Keep-Alive\r\n"
        "\r\n";
    Request request_1{request_1_str};
    CHECK(!request_1.IsValid());

    /* method not supported */
    std::string request_2_str =
        "PUNCH /hello.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/4.0\r\n"
        "Host: www.tutorialspoint.com\r\n"
        "Connection: Keep-Alive\r\n"
        "\r\n";
    Request request_2{request_2_str};
    CHECK(!request_2.IsValid());

    /* version not supported */
    std::string request_3_str =
        "GET /hello.html HTTP/2.0\r\n"
        "User-Agent: Mozilla/4.0\r\n"
        "Host: www.tutorialspoint.com\r\n"
        "Connection: Keep-Alive\r\n"
        "\r\n";
    Request request_3{request_3_str};
    CHECK(!request_3.IsValid());

    /* the bare minimum valid request */
    std::string request_4_str =
        "GET /hello.html HTTP/1.1\r\n"
        "\r\n";
    Request request_4{request_4_str};
    CHECK(request_4.IsValid());
    CHECK(request_4.GetMethod() == Method::GET);
    CHECK(request_4.GetVersion() == Version::HTTP_1_1);

    /* connection to close request */
    std::string request_5_str =
        "HEAD /hello.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/4.0\r\n"
        "Host: www.tutorialspoint.com\r\n"
        "Connection: close\r\n"
        "\r\n";
    Request request_5{request_5_str};
    CHECK(request_5.IsValid());
    CHECK(request_5.GetMethod() == Method::HEAD);
    CHECK(request_5.ShouldClose());

    /* connection to kepp alive request */
    std::string request_6_str =
        "GET /hello.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/4.0\r\n"
        "Host: www.tutorialspoint.com\r\n"
        "Connection: Keep-Alive\r\n"
        "\r\n";
    Request request_6{request_6_str};
    CHECK(!request_6.ShouldClose());
  }
}
