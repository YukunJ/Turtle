/**
 * @file socket.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 25 2022
 *
 * This is an implementation file implementing the Socket, which acts as either
 * the listener or client
 */
#include "core/socket.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <stdexcept>

#include "core/net_address.h"
#include "log/logger.h"

namespace TURTLE_SERVER {

static constexpr int BACK_LOG = 128;

Socket::Socket(int fd) noexcept : fd_(fd) {}

Socket::Socket(Socket &&other) noexcept {
  fd_ = other.fd_;
  other.fd_ = -1;
}

auto Socket::operator=(Socket &&other) noexcept -> Socket & {
  if (fd_ != -1) {
    close(fd_);
  }
  std::swap(fd_, other.fd_);
  return *this;
}

Socket::~Socket() {
  if (fd_ != -1) {
    close(fd_);
    fd_ = -1;
  }
}

auto Socket::GetFd() const noexcept -> int { return fd_; }

void Socket::Connect(NetAddress &server_address) {
  if (fd_ == -1) {
    CreateByProtocol(server_address.GetProtocol());
  }
  if (connect(fd_, server_address.YieldAddr(), *server_address.YieldAddrLen()) == -1) {
    LOG_ERROR("Socket: Connect() error");
    throw std::logic_error("Socket: Connect() error");
  }
}

void Socket::Bind(NetAddress &server_address, bool set_reusable) {
  if (fd_ == -1) {
    CreateByProtocol(server_address.GetProtocol());
  }
  if (set_reusable) {
    SetReusable();
  }
  if (bind(fd_, server_address.YieldAddr(), *server_address.YieldAddrLen()) == -1) {
    LOG_ERROR("Socket: Bind() error");
    throw std::logic_error("Socket: Bind() error");
  }
}

void Socket::Listen() {
  assert(fd_ != -1 && "cannot Listen() with an invalid fd");
  if (listen(fd_, BACK_LOG) == -1) {
    LOG_ERROR("Socket: Listen() error");
    throw std::logic_error("Socket: Listen() error");
  }
}

auto Socket::Accept(NetAddress &client_address) -> int {
  assert(fd_ != -1 && "cannot Accept() with an invalid fd");
  int client_fd = accept(fd_, client_address.YieldAddr(), client_address.YieldAddrLen());
  if (client_fd == -1) {
    // under high pressure, accept might fail.
    // but server should not fail at this time
    LOG_WARNING("Socket: Accept() error");
  }
  return client_fd;
}

void Socket::SetReusable() {
  assert(fd_ != -1 && "cannot SetReusable() with an invalid fd");
  int yes = 1;
  if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1 ||
      setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof yes) == -1) {
    LOG_ERROR("Socket: SetReusable() error");
    throw std::logic_error("Socket: SetReusable() error");
  }
}

void Socket::SetNonBlocking() {
  assert(fd_ != -1 && "cannot SetNonBlocking() with an invalid fd");
  if (fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK) == -1) {
    LOG_ERROR("Socket: SetNonBlocking() error");
    throw std::logic_error("Socket: SetNonBlocking() error");
  }
}

auto Socket::GetAttrs() -> int {
  assert(fd_ != -1 && "cannot GetAttrs() with an invalid fd");
  return fcntl(fd_, F_GETFL);
}

void Socket::CreateByProtocol(Protocol protocol) {
  if (protocol == Protocol::Ipv4) {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
  } else {
    fd_ = socket(AF_INET6, SOCK_STREAM, 0);
  }
  if (fd_ == -1) {
    LOG_ERROR("Socket: socket() error");
    throw std::logic_error("Socket: socket() error");
  }
}
}  // namespace TURTLE_SERVER
