/**
 * @file cgier.cpp
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 13 2023
 *
 * This is an implementation file implementing the CGIer that forks another
 * process to run the cgi program commanded by the client through http request
 * in a RESTful style
 */

#include "http/cgier.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "http/http_utils.h"

namespace TURTLE_SERVER::HTTP {

auto Cgier::ParseCgier(const std::string &resource_url) noexcept -> Cgier {
  if (resource_url.empty()) {
  }
  return MakeInvalidCgier();
}

auto Cgier::MakeInvalidCgier() noexcept -> Cgier {
  Cgier invalid_cgier{std::string(), std::vector<std::string>()};
  invalid_cgier.valid_ = false;
  return invalid_cgier;
}

Cgier::Cgier(const std::string &path,
             const std::vector<std::string> &arguments) noexcept
    : cgi_program_path_(path), cgi_arguments_(arguments), valid_(true) {}

auto Cgier::Run() -> std::vector<unsigned char> {
  return std::vector<unsigned char>();
}

auto Cgier::IsValid() const noexcept -> bool { return valid_; }

}  // namespace TURTLE_SERVER::HTTP
