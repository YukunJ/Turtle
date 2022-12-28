/**
 * @file socket.cpp
 * @author Yukun J
 * @expectation this file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 25 2022
 *
 * This is an implementation file implementing the Socket, which acts as either
 * the listener or client
 */

#include "socket.h"

#define BACK_LOG 128

namespace TURTLE_SERVER {

Socket::Socket(bool is_ipv4) : fd_(-1) {
  if (is_ipv4) {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
  } else {
    fd_ = socket(AF_INET6, SOCK_STREAM, 0);
  }
  if (fd_ == -1) {
    perror("Socket: socket() error");
    exit(EXIT_FAILURE);
  }
}

Socket::Socket(int fd) : fd_(fd) {}

Socket::Socket(Socket &&other) noexcept {
  fd_ = other.fd_;
  other.fd_ = -1;
}

Socket &Socket::operator=(Socket &&other) noexcept {
  if (fd_ != -1) {
    close(fd_);
  }
  fd_ = other.fd_;
  other.fd_ = -1;
  return *this;
}

Socket::~Socket() {
  if (fd_ != -1) {
    close(fd_);
    fd_ = -1;
  }
}

auto Socket::GetFd() const -> int { return fd_; }

void Socket::Connect(NetAddress &server_address) {
  assert(fd_ != -1 && "cannot Connect() with an invalid fd");
  if (connect(fd_, server_address.YieldAddr(),
              *server_address.YieldAddrLen()) == -1) {
    perror("Socket: Connect() error");
    exit(EXIT_FAILURE);
  }
}

void Socket::Bind(NetAddress &server_address) {
  assert(fd_ != -1 && "cannot Bind() with an invalid fd");
  if (bind(fd_, server_address.YieldAddr(), *server_address.YieldAddrLen()) ==
      -1) {
    perror("Socket: Bind() error");
    exit(EXIT_FAILURE);
  }
}

void Socket::Listen() {
  assert(fd_ != -1 && "cannot Listen() with an invalid fd");
  if (listen(fd_, BACK_LOG) == -1) {
    perror("Socket: Listen() error");
    exit(EXIT_FAILURE);
  }
}

auto Socket::Accept(NetAddress &client_address) -> int {
  assert(fd_ != -1 && "cannot Accept() with an invalid fd");
  int client_fd = -1;
  if ((client_fd = accept(fd_, client_address.YieldAddr(),
                          client_address.YieldAddrLen())) == -1) {
    perror("Socket: Accept() error");
    exit(EXIT_FAILURE);
  }
  return client_fd;
}

void Socket::SetReusable() {
  assert(fd_ != -1 && "cannot SetReusable() with an invalid fd");
  int yes = 1;
  if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1 ||
      setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof yes) == -1) {
    perror("Socket: SetReusable() error");
    exit(EXIT_FAILURE);
  }
}

void Socket::SetNonBlocking() {
  assert(fd_ != -1 && "cannot SetNonBlocking() with an invalid fd");
  if (fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK) == -1) {
    perror("Socket: SetNonBlocking() error");
    exit(EXIT_FAILURE);
  }
}

}  // namespace TURTLE_SERVER
