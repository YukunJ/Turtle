/**
 * @file buffer.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 24 2022
 *
 * This is a header file implementing the char Buffer to push-in and pop-out
 * bytes in order
 */

#ifndef SRC_INCLUDE_BUFFER_H_
#define SRC_INCLUDE_BUFFER_H_

#include <string>
#include <vector>

#define INITIAL_BUFFER_CAPACITY 1024

namespace TURTLE_SERVER {

/**
 * This Buffer abstracts an underlying dynamic char array
 * that allows pushing in byte data from two ends
 * NOT thread-safe
 * */
class Buffer {
  friend class Connection;

 public:
  explicit Buffer(size_t initial_capacity = INITIAL_BUFFER_CAPACITY);

  ~Buffer() = default;

  void Append(const char *new_char_data, size_t data_size);

  void Append(const std::string &new_str_data);

  void AppendHead(const char *new_char_data, size_t data_size);

  void AppendHead(const std::string &new_str_data);

  auto Size() const -> size_t;

  auto ToCString() -> char *;

  auto ToString() const -> std::string;

  void Clear();

 private:
  std::vector<char> buf_;
};

}  // namespace TURTLE_SERVER
#endif  // SRC_INCLUDE_BUFFER_H_
