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

#ifndef SRC_INCLUDE_CORE_CONNECTION_H_
#define SRC_INCLUDE_CORE_CONNECTION_H_

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "core/buffer.h"
#include "core/socket.h"
#include "core/utils.h"

namespace TURTLE_SERVER {

constexpr static int TEMP_BUF_SIZE = 2048;

class Looper;

/**
 * This Connection class encapsulates a TCP client connection
 * It could be set a custom callback function when new messages arrive
 * and it contains information about the monitoring events and return events
 * so that Poller could manipulate and epoll based on this Connection class
 * */
class Connection {
 public:
  explicit Connection(std::unique_ptr<Socket> socket);
  ~Connection() = default;

  NON_COPYABLE(Connection);

  auto GetFd() const noexcept -> int;
  auto GetSocket() noexcept -> Socket *;

  /* for Poller */
  void SetEvents(uint32_t events);
  auto GetEvents() const noexcept -> uint32_t;
  void SetRevents(uint32_t revents);
  auto GetRevents() const noexcept -> uint32_t;

  void SetCallback(const std::function<void(Connection *)> &callback);
  auto GetCallback() noexcept -> std::function<void()>;

  /* for Buffer */
  auto FindAndPopTill(const std::string &target) -> std::optional<std::string>;
  auto GetReadBufferSize() const noexcept -> size_t;
  auto GetWriteBufferSize() const noexcept -> size_t;
  void WriteToReadBuffer(const unsigned char *buf, size_t size);
  void WriteToWriteBuffer(const unsigned char *buf, size_t size);
  void WriteToReadBuffer(const std::string &str);
  void WriteToWriteBuffer(const std::string &str);
  void WriteToWriteBuffer(std::vector<unsigned char> &&other_buf);

  auto Read() const noexcept -> const unsigned char *;
  auto ReadAsString() const noexcept -> std::string;

  /* return std::pair<How many bytes read, whether the client exits> */
  auto Recv() -> std::pair<ssize_t, bool>;
  void Send();
  void ClearReadBuffer() noexcept;
  void ClearWriteBuffer() noexcept;

  void SetLooper(Looper *looper) noexcept;
  auto GetLooper() noexcept -> Looper *;

 private:
  Looper *owner_looper_{nullptr};
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Buffer> read_buffer_;
  std::unique_ptr<Buffer> write_buffer_;
  uint32_t events_{0};
  uint32_t revents_{0};
  std::function<void()> callback_{nullptr};
};

}  // namespace TURTLE_SERVER
#endif  // SRC_INCLUDE_CORE_CONNECTION_H_
