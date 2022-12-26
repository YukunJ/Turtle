/**
 * @file buffer.cpp
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 24 2022
 *
 * This is an implementation file implementing the char Buffer to push-in and
 * pop-out bytes in order
 */

#include "buffer.h"

namespace TURTLE_SERVER {

Buffer::Buffer(size_t initial_capacity) { buf_.reserve(initial_capacity); }

void Buffer::Append(const char *new_char_data, size_t data_size) {
  buf_.insert(buf_.end(), new_char_data, new_char_data + data_size);
}

void Buffer::Append(const std::string &new_str_data) {
  Append(new_str_data.c_str(), new_str_data.size());
}

void Buffer::AppendHead(const char *new_char_data, size_t data_size) {
  buf_.insert(buf_.begin(), new_char_data, new_char_data + data_size);
}

void Buffer::AppendHead(const std::string &new_str_data) {
  AppendHead(new_str_data.c_str(), new_str_data.size());
}

auto Buffer::Size() const -> size_t { return buf_.size(); }

auto Buffer::ToCString() -> char * {
  return reinterpret_cast<char *>(buf_.data());
}

auto Buffer::ToString() const -> std::string {
  return {buf_.begin(), buf_.end()};
}

void Buffer::Clear() { buf_.clear(); }

}  // namespace TURTLE_SERVER