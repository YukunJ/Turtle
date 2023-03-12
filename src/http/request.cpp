/**
 * @file request.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 30 2022
 *
 * This is a header file implementing the HTTP request
 */
#include "http/request.h"

#include <algorithm>

#include "http/header.h"
#include "http/http_utils.h"
namespace TURTLE_SERVER::HTTP {

Request::Request(Method method, Version version, std::string resource_url, const std::vector<Header> &headers) noexcept
    : method_(method), version_(version), resource_url_(std::move(resource_url)), headers_(headers), is_valid_(true) {}

Request::Request(const std::string &request_str) noexcept {
  auto lines = Split(request_str, CRLF);
  if (lines.size() < 2 || !lines.back().empty()) {
    invalid_reason_ = "Request format is wrong.";
    return;
  }
  /* the ending of a request should be '\r\n\r\n'
   * which is split to empty string in the last token */
  if (!lines.back().empty()) {
    invalid_reason_ = "Ending of the request is not \r\n\r\n";
    return;
  }
  lines.pop_back();
  bool request_line_parse_success = ParseRequestLine(lines[0]);
  if (!request_line_parse_success) {
    return;
  }
  lines.erase(lines.begin());
  for (const auto &line : lines) {
    Header header{line};
    if (!header.IsValid()) {
      invalid_reason_ = "Fail to parse header line: " + line;
      return;
    }
    ScanHeader(header);
    headers_.push_back(std::move(header));
  }
  is_valid_ = true;
}

auto Request::ShouldClose() const noexcept -> bool { return should_close_; }

auto Request::IsValid() const noexcept -> bool { return is_valid_; }

auto Request::GetMethod() const noexcept -> Method { return method_; }

auto Request::GetVersion() const noexcept -> Version { return version_; }

auto Request::GetResourceUrl() const noexcept -> std::string { return resource_url_; }

auto Request::GetHeaders() const noexcept -> std::vector<Header> { return headers_; }

auto Request::GetInvalidReason() const noexcept -> std::string { return invalid_reason_; }

auto Request::ParseRequestLine(const std::string &request_line) -> bool {
  auto tokens = Split(request_line, SPACE);
  if (tokens.size() != 3) {
    invalid_reason_ = "Invalid first request headline: " + request_line;
    return false;
  }
  method_ = ToMethod(tokens[0]);
  if (method_ == Method::UNSUPPORTED) {
    invalid_reason_ = "Unsupported method: " + tokens[0];
    return false;
  }
  version_ = ToVersion(tokens[2]);
  if (version_ == Version::UNSUPPORTED) {
    invalid_reason_ = "Unsupported version: " + tokens[2];
    return false;
  }
  // default route to index.html
  resource_url_ =
      (tokens[1].empty() || tokens[1].at(tokens[1].size() - 1) == '/') ? tokens[1] + DEFAULT_ROUTE : tokens[1];
  return true;
}

void Request::ScanHeader(const Header &header) {
  /* currently only scan for whether the connection should be closed after
   * service */
  auto key = Format(header.GetKey());
  if (key == Format(HEADER_CONNECTION)) {
    auto value = Format(header.GetValue());
    if (value == Format(CONNECTION_KEEP_ALIVE)) {
      should_close_ = false;
    }
  }
}

auto operator<<(std::ostream &os, const Request &request) -> std::ostream & {
  if (!request.IsValid()) {
    os << "Request is not invalid." << std::endl;
    os << "Reason: " << request.invalid_reason_ << std::endl;
  } else {
    os << "Request is valid." << std::endl;
    os << "Method: " << METHOD_TO_STRING.at(request.method_) << std::endl;
    os << "HTTP Version: " << VERSION_TO_STRING.at(request.version_) << std::endl;
    os << "Resource Url: " << request.resource_url_ << std::endl;
    os << "Connection Keep Alive: " << ((request.should_close_) ? "False" : "True") << std::endl;
    os << "Headers: " << std::endl;
    auto headers = request.GetHeaders();
    std::for_each(headers.begin(), headers.end(), [&](auto header) { os << header.Serialize(); });
  }
  return os;
}

}  // namespace TURTLE_SERVER::HTTP
