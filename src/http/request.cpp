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

namespace TURTLE_SERVER::HTTP {

Request::Request(Method method, Version version, std::string resource_url,
                 const std::vector<Header>& headers) noexcept
    : method_(method),
      version_(version),
      resource_url_(std::move(resource_url)),
      headers_(headers),
      is_valid_(true) {}

Request::Request(const std::string& request_str) noexcept {
  auto lines = Split(request_str, CRLF);
  if (lines.size() < 2 || !lines.back().empty()) {
    invalid_reason_ = "Request format is wrong.";
    return;
  }
  lines.pop_back();
  bool request_line_parse_success = ParseRequestLine(lines[0]);
  if (!request_line_parse_success) {
    return;
  }
  lines.erase(lines.begin());
  for (const auto& line : lines) {
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

auto Request::IsValid() const -> bool { return is_valid_; }

auto Request::GetMethod() const -> Method { return method_; }

auto Request::GetVersion() const -> Version { return version_; }

auto Request::GetResourceUrl() const -> std::string { return resource_url_; }

auto Request::GetHeaders() const -> std::vector<Header> { return headers_; }

auto Request::GetInvalidReason() const -> std::string {
  return invalid_reason_;
}

auto Request::ParseRequestLine(const std::string& request_line) -> bool {
  auto tokens = Split(request_line, SPACE);
  if (tokens.size() != 3) {
    invalid_reason_ = "Invalid first request headline: " + request_line;
    return false;
  }
  method_ = To_Method(tokens[0]);
  if (method_ == Method::UNSUPPORTED) {
    invalid_reason_ = "Unsupported method: " + tokens[0];
    return false;
  }
  version_ = To_Version(tokens[2]);
  if (version_ == Version::UNSUPPORTED) {
    invalid_reason_ = "Unsupported version: " + tokens[2];
    return false;
  }
  // default route to index.html
  resource_url_ = (tokens[1].at(tokens[1].size() - 1) == '/')
                      ? tokens[1] + DEFAULT_ROUTE
                      : tokens[1];
  return true;
}

void Request::ScanHeader(const Header& header) {
  auto key = header.GetKey();
  Trim(key);
  To_Upper(key);
  if (key == HEADER_CONNECTION) {
    auto value = header.GetValue();
    Trim(value);
    To_Upper(value);
    if (value == CONNECTION_KEEP_ALIVE) {
      should_close_ = false;
    }
  }
}

std::ostream& operator<<(std::ostream& os, const Request& request) {
  if (!request.IsValid()) {
    os << "Request is not invalid." << std::endl;
    os << "Reason: " << request.invalid_reason_ << std::endl;
  } else {
    os << "Request is valid." << std::endl;
    os << "Method: " << METHOD_TO_STRING.at(request.method_) << std::endl;
    os << "HTTP Version: " << VERSION_TO_STRING.at(request.version_)
       << std::endl;
    os << "Resource Url: " << request.resource_url_ << std::endl;
    os << "Connection Keep Alive: "
       << ((request.should_close_) ? "False" : "True") << std::endl;
    os << "Headers: " << std::endl;
    auto headers = request.GetHeaders();
    std::for_each(headers.begin(), headers.end(),
                  [&](auto header) { os << header.Serialize(); });
  }
  return os;
}

}  // namespace TURTLE_SERVER::HTTP
