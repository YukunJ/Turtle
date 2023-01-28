/**
 * @file poller.h
 * @author Yukun J
 * @expectation this file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 25 2022
 *
 * This is a header file implementing the Poller
 * which actively does epolling on a collection of socket descriptors to be
 * monitored
 */
#ifndef SRC_INCLUDE_POLLER_H_
#define SRC_INCLUDE_POLLER_H_

#ifdef OS_LINUX  // Linux Epoll
#include <sys/epoll.h>
#elif OS_MAC  // Mac KQueue
#include <sys/event.h>
#endif

#include <memory>
#include <vector>

#include "core/utils.h"

namespace TURTLE_SERVER {

/* the default maximum number of events to be listed on epoll tree */
static constexpr int DEFAULT_EVENTS_LISTENED = 1024;

#ifdef OS_LINUX  // Linux Epoll
static constexpr unsigned POLL_ADD = EPOLL_CTL_ADD;
static constexpr unsigned POLL_READ = EPOLLIN;
static constexpr unsigned POLL_ET = EPOLLET;
#elif OS_MAC  // Mac KQueue
static constexpr unsigned POLL_ADD =
    EVFILT_READ;  // a bit awkward but this is how kqueue works
static constexpr unsigned POLL_READ = EV_ADD;
static constexpr unsigned POLL_ET = EV_CLEAR;
#endif

class Connection;

/**
 * This Poller acts at the socket monitor that actively polling on connections
 * */
class Poller {
 public:
  explicit Poller(uint64_t poll_size = DEFAULT_EVENTS_LISTENED);

  ~Poller();

  NON_COPYABLE(Poller);

  void AddConnection(Connection *conn);

  // timeout in milliseconds
  auto Poll(int timeout = -1) -> std::vector<Connection *>;

  auto GetPollSize() const noexcept -> uint64_t;

 private:
  int poll_fd_;
  uint64_t poll_size_;
#ifdef OS_LINUX
  struct epoll_event *poll_events_{nullptr};
#elif OS_MAC
  struct kevent *poll_events_{nullptr};
#endif
};
}  // namespace TURTLE_SERVER
#endif  // SRC_INCLUDE_POLLER_H_
