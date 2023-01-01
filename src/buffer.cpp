/**
 * @file buffer.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 24 2022
 *
 * This is an implementation file implementing the char Buffer to push-in and
 * pop-out bytes in order
 */

#include "buffer.h"
namespace TURTLE_SERVER {

Buffer::Buffer(size_t initial_capacity) { buf_.reserve(initial_capacity); }

void Buffer::Append(const unsigned char *new_char_data, size_t data_size) {
  buf_.insert(buf_.end(), new_char_data, new_char_data + data_size);
}

void Buffer::Append(const std::string &new_str_data) {
  Append(reinterpret_cast<const unsigned char *>(new_str_data.c_str()),
         new_str_data.size());
}

void Buffer::AppendHead(const unsigned char *new_char_data, size_t data_size) {
  buf_.insert(buf_.begin(), new_char_data, new_char_data + data_size);
}

void Buffer::AppendHead(const std::string &new_str_data) {
  AppendHead(reinterpret_cast<const unsigned char *>(new_str_data.c_str()),
             new_str_data.size());
}

auto Buffer::FindAndPop(const std::string &target)
    -> std::optional<std::string> {
  std::optional<std::string> res = std::nullopt;
  auto curr_content = ToString();
  auto pos = curr_content.find(target);
  if (pos != std::string::npos) {
    res = curr_content.substr(0, pos + target.size());
    buf_.erase(buf_.begin(), buf_.begin() + pos + target.size());
  }
  return res;
}

auto Buffer::Size() const -> size_t { return buf_.size(); }

auto Buffer::Data() -> const unsigned char * { return buf_.data(); }

auto Buffer::ToString() const -> std::string {
  return {buf_.begin(), buf_.end()};
}

void Buffer::Clear() { buf_.clear(); }

}  // namespace TURTLE_SERVER
