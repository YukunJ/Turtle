/**
 * @file http_utils.cpp
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 3 2023
 *
 * This is an implementation file for http module's constant enum definitions
 * and utility functions
 */

#include "http/http_utils.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
namespace TURTLE_SERVER::HTTP {

auto ToMethod(const std::string& method_str) noexcept -> Method {
  auto method_str_fomatted = Format(method_str);
  if (method_str_fomatted == METHOD_TO_STRING.at(Method::GET)) {
    return Method::GET;
  }
  return Method::UNSUPPORTED;
}

auto ToVersion(const std::string& version_str) noexcept -> Version {
  auto version_str_formatted = Format(version_str);
  if (version_str_formatted == VERSION_TO_STRING.at(Version::HTTP_1_1)) {
    return Version::HTTP_1_1;
  }
  return Version::UNSUPPORTED;
}

auto Split(const std::string& str, const char* delim) noexcept
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

auto Join(const std::vector<std::string>& tokens, const char* delim) noexcept
    -> std::string {
  std::stringstream str_stream;
  for (const auto& token : tokens) {
    str_stream << token << delim;
  }
  return str_stream.str();
}

auto Trim(const std::string& str, const char* delim) noexcept -> std::string {
  size_t r_found = str.find_last_not_of(delim);
  if (r_found == std::string::npos) {
    return {};
  }
  size_t l_found = str.find_first_not_of(delim);
  return str.substr(l_found, r_found - l_found + 1);
}

auto ToUpper(std::string str) noexcept -> std::string {
  std::transform(str.begin(), str.end(), str.begin(),
                 [](char c) { return std::toupper(c); });
  return str;
}

auto Format(const std::string& str) noexcept -> std::string {
  return ToUpper(Trim(str));
}

auto IsDirectoryExists(const std::string& directory_path) noexcept -> bool {
  return std::filesystem::is_directory(directory_path);
}

auto IsFileExists(const std::string& file_path) noexcept -> bool {
  return std::filesystem::exists(file_path);
}

auto CheckFileSize(const std::string& file_path) noexcept -> size_t {
  assert(IsFileExists(file_path));
  return std::filesystem::file_size(file_path);
}

void LoadFile(const std::string& file_path,
              std::vector<char>& buffer) noexcept {  // NOLINT
  size_t file_size = CheckFileSize(file_path);
  std::ifstream file(file_path);
  buffer.resize(file_size);
  assert(file.is_open());
  file.read(&buffer[0], static_cast<std::streamsize>(file_size));
}

}  // namespace TURTLE_SERVER::HTTP
