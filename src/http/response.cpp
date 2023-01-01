/**
 * @file response.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 31 2022
 *
 * This is an implementation file implementing the HTTP response
 */

#include "http/response.h"

namespace TURTLE_SERVER::HTTP {

auto Response::Make200Response(bool should_close,
                               const std::string& resource_url) -> Response {
  return {OK, should_close, resource_url};
}

auto Response::Make400Response() -> Response {
  return {BAD_REQUEST, true, std::nullopt};
}

auto Response::Make404Response() -> Response {
  return {NOT_FOUND, true, std::nullopt};
}

auto Response::Make503Response() -> Response {
  return {SERVICE_UNAVAILABLE, true, std::nullopt};
}

Response::Response(const std::string& status_code, bool should_close,
                   std::optional<std::string> resource_url)
    : should_close_(should_close), resource_url_(std::move(resource_url)) {
  // construct the status line
  std::stringstream str_stream;
  str_stream << TURTLE_HTTP_VERSION << SPACE << status_code;
  status_line_ = str_stream.str();
  // add necessary headers
  headers_.emplace_back("Server", SERVER);
  headers_.emplace_back(
      HEADER_CONNECTION,
      ((should_close_) ? CONNECTION_CLOSE : CONNECTION_KEEP_ALIVE));
  // if resource is specified and available
  if (resource_url_.has_value()) {
    size_t content_length = CheckFileSize(resource_url_.value());
    headers_.emplace_back(HEADER_CONTENT_LENGTH,
                          std::to_string(content_length));
    LoadFile(resource_url_.value(), body_);
  } else {
    headers_.emplace_back(HEADER_CONTENT_LENGTH, "0");
  }
}

void Response::Serialize(std::vector<unsigned char>& buffer) {  // NOLINT
  // construct everything before body
  std::stringstream str_stream;
  str_stream << status_line_ << CRLF;
  for (const auto& header : headers_) {
    str_stream << header.Serialize();
  }
  str_stream << CRLF;
  std::string response_head = str_stream.str();
  buffer.insert(buffer.end(), response_head.begin(), response_head.end());
  if (!body_.empty()) {
    buffer.insert(buffer.end(), body_.begin(), body_.end());
  }
}
}  // namespace TURTLE_SERVER::HTTP
