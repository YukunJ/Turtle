/**
 * @file response.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 31 2022
 *
 * This is a header file implementing the HTTP response
 */

#ifndef SRC_INCLUDE_HTTP_RESPONSE_H_
#define SRC_INCLUDE_HTTP_RESPONSE_H_

#include <optional>
#include <string>
#include <vector>

namespace TURTLE_SERVER::HTTP {

class Header;

/**
 * The HTTP Response class
 * use vector of char to be able to contain binary data
 */
class Response {
 public:
  /* 200 OK response */
  static auto Make200Response(bool should_close, std::optional<std::string> resource_url) -> Response;
  /* 400 Bad Request response, close connection */
  static auto Make400Response() noexcept -> Response;
  /* 404 Not Found response, close connection */
  static auto Make404Response() noexcept -> Response;
  /* 503 Service Unavailable response, close connection */
  static auto Make503Response() noexcept -> Response;

  Response(const std::string &status_code, bool should_close, std::optional<std::string> resource_url);

  /* no content, content should separately be loaded */
  void Serialize(std::vector<unsigned char> &buffer);  // NOLINT

  auto GetHeaders() -> std::vector<Header>;

  auto ChangeHeader(const std::string &key, const std::string &new_value) noexcept -> bool;

 private:
  std::string status_line_;
  bool should_close_;
  std::vector<Header> headers_;
  std::optional<std::string> resource_url_;
  std::vector<unsigned char> body_;
};

}  // namespace TURTLE_SERVER::HTTP

#endif  // SRC_INCLUDE_HTTP_RESPONSE_H_
