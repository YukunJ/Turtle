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

namespace TURTLE_SERVER::HTTP {

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

void Trim(std::string& str, const char* delim) {  // NOLINT
  size_t r_found = str.find_last_not_of(delim);
  if (r_found == std::string::npos) {
    str.clear();
    return;
  }
  str.erase(r_found + 1);
  size_t l_found = str.find_first_not_of(delim);
  str.erase(0, l_found);
}
}  // namespace TURTLE_SERVER::HTTP
