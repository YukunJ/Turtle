/**
 * @file poller.cpp
 * @author Yukun J
 * @expectation this file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 25 2022
 *
 * This is an implementation file implementing the Poller
 * which actively does epolling on a collection of socket descriptors to be
 * monitored
 */

#include "poller.h"

namespace TURTLE_SERVER {

Poller::Poller(uint64_t poll_size) : poll_size_(poll_size) {
  poll_fd_ = epoll_create1(0);
  if (poll_fd_ == -1) {
    perror("Poller: epoll_create1() error");
    exit(EXIT_FAILURE);
  }
  poll_events_ = new struct epoll_event[poll_size];
  memset(poll_events_, 0, poll_size_ * sizeof(struct epoll_event));
}

Poller::~Poller() {
  if (poll_fd_ != -1) {
    close(poll_fd_);
    delete[] poll_events_;
    poll_fd_ = -1;
  }
}

void Poller::AddConnection(Connection *conn) {
  assert(conn->GetFd() != -1 && "cannot AddConnection() with an invalid fd");
  struct epoll_event event;
  memset(&event, 0, sizeof(struct epoll_event));
  event.data.ptr = conn;
  event.events = conn->GetEvents();
  int ret_val = epoll_ctl(poll_fd_, EPOLL_CTL_ADD, conn->GetFd(), &event);
  if (ret_val == -1) {
    perror("Poller: epoll_ctl add error");
    exit(EXIT_FAILURE);
  }
  conn->SetInPoller(true);
}

auto Poller::Poll(int timeout) -> std::vector<Connection *> {
  std::vector<Connection *> events_happen;
  int ready = epoll_wait(poll_fd_, poll_events_, poll_size_, timeout);
  if (ready == -1) {
    perror("Poller: Poll() error");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < ready; i++) {
    Connection *ready_connection =
        reinterpret_cast<Connection *>(poll_events_[i].data.ptr);
    ready_connection->SetRevents(poll_events_[i].events);
    events_happen.emplace_back(ready_connection);
  }
  return events_happen;
}

auto Poller::GetPollSize() const -> uint64_t { return poll_size_; }
}  // namespace TURTLE_SERVER