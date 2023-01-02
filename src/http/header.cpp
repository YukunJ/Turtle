/**
 * @file header.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 29 2022
 *
 * This is an implementation file implementing the HTTP Header class, which is
 * essentially a key-value pair
 */

#include "http/header.h"

#include "http/http_utils.h"

namespace TURTLE_SERVER::HTTP {

Header::Header(const std::string &key, const std::string &value)
    : key_(key), value_(value) {}

Header::Header(std::string &&key, std::string &&value)
    : key_(std::move(key)), value_(std::move(value)) {}

Header::Header(const std::string &line) {
  auto tokens = Split(line, COLON);
  if (tokens.size() < 2) {
    valid_ = false;
    return;
  }
  key_ = std::move(tokens[0]);
  tokens.erase(tokens.begin());
  std::string value = Join(tokens, COLON);
  value_ = std::move(value);
}

Header::Header(Header &&other) noexcept
    : key_(std::move(other.key_)),
      value_(std::move(other.value_)),
      valid_(other.valid_) {}

Header &Header::operator=(Header &&other) noexcept {
  key_ = std::move(other.key_);
  value_ = std::move(other.value_);
  valid_ = other.valid_;
  return *this;
}

auto Header::IsValid() const -> bool { return valid_; }

auto Header::GetKey() const -> std::string { return key_; }

auto Header::GetValue() const -> std::string { return value_; }

auto Header::Serialize() const -> std::string {
  return key_ + COLON + value_ + CRLF;
}

std::ostream &operator<<(std::ostream &os, const Header &header) {
  os << "HTTP Header contains:" << std::endl;
  os << "Key: " << header.GetKey() << std::endl;
  os << "Value: " << header.GetValue() << std::endl;
  os << "IsValid: " << ((header.IsValid()) ? "True" : "False") << std::endl;
  os << "Serialize to: " << header.Serialize();
  return os;
}

}  // namespace TURTLE_SERVER::HTTP
