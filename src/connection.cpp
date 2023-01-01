/**
 * @file connection.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 25 2022
 *
 * This is an implementation file implementing the TCP connection, which
 * supports callback functionality
 */

#include "connection.h"

namespace TURTLE_SERVER {

Connection::Connection(Looper *looper, std::unique_ptr<Socket> socket)
    : looper_(looper),
      socket_(std::move(socket)),
      read_buffer_(std::make_unique<Buffer>()),
      write_buffer_(std::make_unique<Buffer>()),
      events_(0),
      revents_(0) {}

auto Connection::GetFd() const -> int { return socket_->GetFd(); }

auto Connection::GetSocket() -> Socket * { return socket_.get(); }

void Connection::SetEvents(uint32_t events) { events_ = events; }

auto Connection::GetEvents() const -> uint32_t { return events_; }

void Connection::SetRevents(uint32_t revents) { revents_ = revents; }

auto Connection::GetRevents() const -> uint32_t { return revents_; }

void Connection::SetInPoller(bool in_poller) { in_poller_ = in_poller; }

auto Connection::InPoller() const -> bool { return in_poller_; }

void Connection::SetCallback(
    const std::function<void(Connection *)> &callback) {
  callback_ = [callback, this] { return callback(this); };
}

auto Connection::GetCallback() -> std::function<void()> { return callback_; }

auto Connection::GetLooper() -> Looper * { return looper_; }

auto Connection::GetReadBuffer() -> Buffer * { return read_buffer_.get(); }

auto Connection::GetWriteBuffer() -> Buffer * { return write_buffer_.get(); }

auto Connection::GetReadBufferSize() -> size_t { return read_buffer_->Size(); }

auto Connection::GetWriteBufferSize() -> size_t {
  return write_buffer_->Size();
}

void Connection::WriteToReadBuffer(const char *buf, size_t size) {
  read_buffer_->Append(buf, size);
}

void Connection::WriteToWriteBuffer(const char *buf, size_t size) {
  write_buffer_->Append(buf, size);
}

void Connection::WriteToReadBuffer(const std::string &str) {
  read_buffer_->Append(str);
}

void Connection::WriteToWriteBuffer(const std::string &str) {
  write_buffer_->Append(str);
}

auto Connection::Read() -> const unsigned char * {
  return read_buffer_->Data();
}

auto Connection::ReadAsString() const -> std::string {
  return read_buffer_->ToString();
}

auto Connection::Recv() -> std::pair<ssize_t, bool> {
  // read all available bytes, since Edge-trigger
  int from_fd = GetFd();
  ssize_t read = 0;
  char buf[TEMP_BUF_SIZE + 1];
  memset(buf, 0, sizeof(buf));
  while (true) {
    ssize_t curr_read = recv(from_fd, buf, TEMP_BUF_SIZE, 0);
    if (curr_read > 0) {
      read += curr_read;
      WriteToReadBuffer(buf, curr_read);
      memset(buf, 0, sizeof(buf));
    } else if (curr_read == 0) {
      // the client has exit
      std::cout << "Client exits: " << from_fd << std::endl;
      return {read, true};
    } else if (curr_read == -1 && errno == EINTR) {
      // normal interrupt
      continue;
    } else if (curr_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      // all data read
      break;
    } else {
      perror("HandleConnection: recv() error");
      return {read, true};
    }
  }
  return {read, false};
}

void Connection::Send() {
  // robust write
  ssize_t curr_write = 0;
  ssize_t write;
  const ssize_t to_write = GetWriteBufferSize();
  const unsigned char *buf = write_buffer_->Data();
  while (curr_write < to_write) {
    if ((write = send(GetFd(), buf + curr_write, to_write - curr_write, 0)) <=
        0) {
      if (errno != EINTR || errno != EAGAIN) {
        ClearWriteBuffer();
        return;
      }
      write = 0;
    }
    curr_write += write;
  }
  ClearWriteBuffer();
}

void Connection::ClearReadBuffer() { read_buffer_->Clear(); }

void Connection::ClearWriteBuffer() { write_buffer_->Clear(); }

}  // namespace TURTLE_SERVER
