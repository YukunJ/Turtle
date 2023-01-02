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

#include <sys/epoll.h>

#include <memory>
#include <vector>

#include "connection.h"
#include "utils.h"


namespace TURTLE_SERVER {

/* the default maximum number of events to be listed on epoll tree */
static constexpr int DEFAULT_EVENTS_LISTENED = 1024;

/**
 * This Poller acts at the socket monitor that actively polling on connections
 * */
class Poller {
 public:
  explicit Poller(uint64_t poll_size = DEFAULT_EVENTS_LISTENED);

  ~Poller();

  NON_COPYABLE(Poller);

  void AddConnection(Connection *conn);

  auto Poll(int timeout = -1) -> std::vector<Connection *>;

  auto GetPollSize() const noexcept -> uint64_t;

 private:
  int poll_fd_;
  struct epoll_event *poll_events_{nullptr};
  uint64_t poll_size_;
};
}  // namespace TURTLE_SERVER
#endif  // SRC_INCLUDE_POLLER_H_
