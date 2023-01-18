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
#include <sys/wait.h>
#include <unistd.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <thread>  // NOLINT

#include "http/http_utils.h"
namespace TURTLE_SERVER::HTTP {

auto Cgier::ParseCgier(const std::string &resource_url) noexcept -> Cgier {
  if (resource_url.empty() || !IsCgiRequest(resource_url)) {
    return MakeInvalidCgier();
  }
  // find the first & after the cgi-bin/ to fetch out cgi program path
  auto cgi_pos = resource_url.find(CGI_BIN);
  auto cgi_separator = resource_url.find(PARAMETER_SEPARATOR, cgi_pos);
  auto cgi_path = resource_url.substr(0, cgi_separator);
  auto arguments =
      Split(resource_url.substr(cgi_separator + 1), PARAMETER_SEPARATOR);
  return Cgier(cgi_path, arguments);
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
  assert(valid_);
  std::vector<unsigned char> cgi_result;
  // unique shared filename within one Cgier
  // when communicating between parent and child
  std::stringstream ssr;
  ssr << CGI_PREFIX << UNDERSCORE << std::this_thread::get_id() << ".txt";
  std::string shared_file_name = ssr.str();
  int fd = open(shared_file_name.c_str(), O_RDWR | O_APPEND | O_CREAT,
                READ_WRITE_PERMISSION);
  if (fd == -1) {
    std::string error = "fail to create/open the file " + shared_file_name;
    return {error.begin(), error.end()};
  }
  pid_t pid = fork();
  if (pid == -1) {
    std::string error = "fail to fork()";
    return {error.begin(), error.end()};
  }
  if (pid == 0) {
    // child

    // link cgi program's stdout to the shared file
    dup2(fd, STDOUT_FILENO);
    close(fd);

    // build argument lists for the cgi program
    char **cgi_argv = BuildArgumentList();

    // walk into cig program
    if (execve(cgi_program_path_.c_str(), cgi_argv, nullptr) < 0) {
      // only reach here when execve fails
      perror("fail to execve()");
      exit(1);  // exit child process
    }
  } else {
    // parent
    close(fd);
    int status;
    // wait and harvest child process
    if (waitpid(pid, &status, 0) == -1) {
      std::string error = "fail to harvest child by waitpid()";
      return {error.begin(), error.end()};
    }
    // load cgi result from the shared file
    LoadFile(shared_file_name, cgi_result);
    // clean it up by deleting shared file
    DeleteFile(shared_file_name);
  }
  return cgi_result;
}

auto Cgier::IsValid() const noexcept -> bool { return valid_; }

auto Cgier::GetPath() const noexcept -> std::string {
  return cgi_program_path_;
}

auto Cgier::BuildArgumentList() -> char ** {
  assert(!cgi_program_path_.empty());
  char **cgi_argv = (char **)calloc(cgi_arguments_.size() + 2, sizeof(char *));
  cgi_argv[0] = (char *)calloc(cgi_program_path_.size() + 1, sizeof(char));
  memcpy(cgi_argv[0], cgi_program_path_.c_str(), cgi_program_path_.size());
  for (size_t i = 0; i < cgi_arguments_.size(); i++) {
    cgi_argv[i + 1] =
        (char *)calloc(cgi_arguments_[i].size() + 1, sizeof(char));
    memcpy(cgi_argv[i + 1], cgi_arguments_[i].c_str(),
           cgi_arguments_[i].size());
  }
  cgi_argv[cgi_arguments_.size() + 1] =
      nullptr;  // indicate the end of arg list
  return cgi_argv;
}

}  // namespace TURTLE_SERVER::HTTP
