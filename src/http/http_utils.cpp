/**
 * @file http_utils.cpp
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 29 2022
 *
 * This is an implementation file for http module's constant enum definitions
 * and utility functions
 */

#include "http/http_utils.h"

#include <cassert>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
namespace TURTLE_SERVER::HTTP {

auto ToMethod(const std::string& method_str) -> Method {
  auto method_str_fomatted = Format(method_str);
  if (method_str_fomatted == "GET") {
    return Method::GET;
  }
  return Method::UNSUPPORTED;
}

auto ToVersion(const std::string& version_str) -> Version {
  auto version_str_formatted = Format(version_str);
  if (version_str_formatted == "HTTP/1.1") {
    return Version::HTTP_1_1;
  }
  return Version::UNSUPPORTED;
}

auto Split(const std::string& str, const char* delim)
    -> std::vector<std::string> {
  std::vector<std::string> tokens;
  if (str.empty()) {
    return tokens;
  }
  std::string token;
  size_t curr = 0;
  size_t next;
  size_t delim_len = strlen(delim);
  while ((next = str.find(delim, curr)) != std::string::npos) {
    tokens.emplace_back(str.substr(curr, next - curr));
    curr = next + delim_len;
  }
  if (curr != str.size()) {
    // one last word
    tokens.emplace_back(str.substr(curr, str.size() - curr));
  }
  return tokens;
}

auto Join(const std::vector<std::string>& tokens, const char* delim)
    -> std::string {
  std::stringstream str_stream;
  for (const auto& token : tokens) {
    str_stream << token << delim;
  }
  return str_stream.str();
}

auto Trim(const std::string& str, const char* delim) -> std::string {
  size_t r_found = str.find_last_not_of(delim);
  if (r_found == std::string::npos) {
    return {};
  }
  size_t l_found = str.find_first_not_of(delim);
  return str.substr(l_found, r_found - l_found + 1);
}

auto ToUpper(std::string str) -> std::string {
  std::transform(str.begin(), str.end(), str.begin(), [](char c) { return std::toupper(c); });
  return str;
}

auto Format(const std::string& str) -> std::string {
  return ToUpper(Trim(str));
}

bool IsDirectoryExists(const std::string& directory_path) {
  return std::filesystem::is_directory(directory_path);
}

bool IsFileExists(const std::string& file_path) {
  return std::filesystem::exists(file_path);
}

size_t CheckFileSize(const std::string& file_path) {
  return std::filesystem::file_size(file_path);
}

void LoadFile(const std::string& file_path,
              std::vector<char>& buffer) {  // NOLINT
  size_t file_size = CheckFileSize(file_path);
  std::ifstream file(file_path);
  buffer.resize(file_size);
  assert(file.is_open());
  file.read(&buffer[0], file_size);
}

}  // namespace TURTLE_SERVER::HTTP
