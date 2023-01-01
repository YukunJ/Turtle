/**
 * @file connection.h
 * @author Yukun J
 * @expectation this file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 25 2022
 *
 * This is a header file implementing the TCP connection, which supports
 * callback functionality
 */

#ifndef SRC_INCLUDE_CONNECTION_H_
#define SRC_INCLUDE_CONNECTION_H_

#include <functional>
#include <memory>
#include <string>
#include <utility>

#include "buffer.h"
#include "socket.h"
#include "utils.h"

#define TEMP_BUF_SIZE 2048

namespace TURTLE_SERVER {

// forward declaration
class Looper;

/**
 * This Connection class encapsulates a TCP client connection
 * It could be set a custom callback function when new messages arrive
 * and it contains information about the monitoring events and return events
 * so that Poller could manipulate and epoll based on this Connection class
 * */
class Connection {
 public:
  explicit Connection(Looper *looper, std::unique_ptr<Socket> socket);
  ~Connection() = default;

  NON_COPYABLE(Connection);

  auto GetFd() const -> int;
  auto GetSocket() -> Socket *;

  /* for Poller */
  void SetEvents(uint32_t events);
  auto GetEvents() const -> uint32_t;
  void SetRevents(uint32_t revents);
  auto GetRevents() const -> uint32_t;

  void SetInPoller(bool in_poller);
  auto InPoller() const -> bool;
  void SetCallback(const std::function<void(Connection *)> &callback);
  auto GetCallback() -> std::function<void()>;

  auto GetLooper() -> Looper *;

  /* for Buffer */
  auto GetReadBuffer() -> Buffer *;
  auto GetWriteBuffer() -> Buffer *;
  auto GetReadBufferSize() -> size_t;
  auto GetWriteBufferSize() -> size_t;
  void WriteToReadBuffer(const unsigned char *buf, size_t size);
  void WriteToWriteBuffer(const unsigned char *buf, size_t size);
  void WriteToReadBuffer(const std::string &str);
  void WriteToWriteBuffer(const std::string &str);

  auto Read() -> const unsigned char *;
  auto ReadAsString() const -> std::string;

  /* return std::pair<How many bytes read, whether the client exits> */
  auto Recv() -> std::pair<ssize_t, bool>;
  void Send();
  void ClearReadBuffer();
  void ClearWriteBuffer();

 private:
  std::unique_ptr<Buffer> read_buffer_;
  std::unique_ptr<Buffer> write_buffer_;
  std::unique_ptr<Socket> socket_;
  Looper *looper_;
  bool in_poller_{false};
  uint32_t events_{};
  uint32_t revents_{};
  std::function<void()> callback_{nullptr};
};

}  // namespace TURTLE_SERVER
#endif  // SRC_INCLUDE_CONNECTION_H_
