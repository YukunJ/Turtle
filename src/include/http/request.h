/**
 * @file request.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 30 2022
 *
 * This is a header file implementing the HTTP request
 */

#ifndef SRC_INCLUDE_HTTP_REQUEST_H_
#define SRC_INCLUDE_HTTP_REQUEST_H_
#include <iostream>
#include <string>
#include <vector>

#include "http/header.h"
#include "http/http_utils.h"
#include "utils.h"
namespace TURTLE_SERVER::HTTP {

/**
 * The (limited GET-only HTTP 1.1) HTTP Request class
 * it contains necessary request line features including method, resource url,
 * http version and since we supports http 1.1, it also cares if the client
 * connection should be kept alive
 */
class Request {
 public:
  Request(Method method, Version version, std::string resource_url,
          const std::vector<Header>& headers) noexcept;
  explicit Request(
      const std::string& request_str) noexcept;  // deserialize method
  NON_COPYABLE(Request);
  auto IsValid() const -> bool;
  auto ShouldClose() const -> bool;
  auto GetInvalidReason() const -> std::string;
  auto GetMethod() const -> Method;
  auto GetVersion() const -> Version;
  auto GetResourceUrl() const -> std::string;
  auto GetHeaders() const -> std::vector<Header>;
  friend std::ostream& operator<<(std::ostream& os, const Request& request);

 private:
  auto ParseRequestLine(const std::string& request_line) -> bool;
  void ScanHeader(const Header& header);
  Method method_;
  Version version_;
  std::string resource_url_;
  std::vector<Header> headers_;
  bool should_close_{true};
  bool is_valid_{false};
  std::string invalid_reason_;
};
}  // namespace TURTLE_SERVER::HTTP

#endif  // SRC_INCLUDE_HTTP_REQUEST_H_
