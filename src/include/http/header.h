/**
 * @file header.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 29 2022
 *
 * This is a header file implementing the HTTP Header class, which is
 * essentially a key-value pair
 */

#ifndef SRC_INCLUDE_HTTP_HEADER_H_
#define SRC_INCLUDE_HTTP_HEADER_H_
#include <iostream>
#include <sstream>
#include <string>

namespace TURTLE_SERVER::HTTP {

/**
 * The HTTP Header in the form of string "key : value"
 */
class Header {
 public:
  Header(const std::string &key, const std::string &value);
  Header(std::string &&key, std::string &&value);
  explicit Header(const std::string &line);  // deserialize method
  Header(const Header &other) = default;
  Header(Header &&other) noexcept;
  auto operator=(const Header &other) -> Header & = default;
  auto operator=(Header &&other) noexcept -> Header &;
  ~Header() = default;

  auto IsValid() const -> bool;
  auto GetKey() const -> std::string;
  auto GetValue() const -> std::string;
  void SetValue(const std::string &new_value) noexcept;
  auto Serialize() const -> std::string;

  friend auto operator<<(std::ostream &os, const Header &header) -> std::ostream &;

 private:
  std::string key_;
  std::string value_;
  bool valid_{true};
};

}  // namespace TURTLE_SERVER::HTTP

#endif  // SRC_INCLUDE_HTTP_HEADER_H_
