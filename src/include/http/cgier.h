/**
 * @file cgier.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 13 2022
 *
 * This is a header file implementing the CGIer that fork another process
 * to run the cgi program commanded by the client through http request in a
 * RESTful style
 */

#ifndef SRC_INCLUDE_HTTP_CGIER_H_
#define SRC_INCLUDE_HTTP_CGIER_H_

#include <string>
#include <vector>

#include "core/utils.h"

namespace TURTLE_SERVER::HTTP {

/**
 * This Cgier runs a client commanded program through traditional 'fork' +
 * 'execve' All the cgi program should reside in a '/cgi-bin' folder in the root
 * directory of the http serving directory parent and child process communicate
 * through a file, where child writes the output to it and parent read it out
 * afterwards
 * */
class Cgier {
 public:
  static auto ParseCgier(const std::string &resource_url) noexcept -> Cgier;
  static auto MakeInvalidCgier() noexcept -> Cgier;
  explicit Cgier(const std::string &path,
                 const std::vector<std::string> &arguments) noexcept;
  auto Run() -> std::vector<unsigned char>;
  auto IsValid() const noexcept -> bool;
  auto GetPath() const noexcept -> std::string;

 private:
  auto BuildArgumentList() -> char **;
  void FreeArgumentList(char** arg_list);
  std::string cgi_program_path_;
  std::vector<std::string> cgi_arguments_;
  bool valid_{true};
};

}  // namespace TURTLE_SERVER::HTTP

#endif  // SRC_INCLUDE_HTTP_CGIER_H_
