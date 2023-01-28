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

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "core/utils.h"

namespace TURTLE_SERVER {

/* default initial underlying capacity of Buffer */
static constexpr size_t INITIAL_BUFFER_CAPACITY = 1024;

/**
 * This Buffer abstracts an underlying dynamic char array
 * that allows pushing in byte data from two ends
 * NOT thread-safe
 * */
class Buffer {
 public:
  explicit Buffer(size_t initial_capacity = INITIAL_BUFFER_CAPACITY);

  ~Buffer() = default;

  Buffer(const Buffer &other) = default;

  Buffer &operator=(const Buffer &other) = default;

  NON_MOVEABLE(Buffer);

  void Append(const unsigned char *new_char_data, size_t data_size);

  void Append(const std::string &new_str_data);

  void Append(std::vector<unsigned char> &&other_buffer);

  void AppendHead(const unsigned char *new_char_data, size_t data_size);

  void AppendHead(const std::string &new_str_data);

  auto FindAndPopTill(const std::string &target) -> std::optional<std::string>;

  auto Size() const noexcept -> size_t;

  auto Data() noexcept -> const unsigned char *;

  auto ToStringView() const noexcept -> std::string_view;

  void Clear() noexcept;

 private:
  std::vector<unsigned char> buf_;
};

}  // namespace TURTLE_SERVER
#endif  // SRC_INCLUDE_BUFFER_H_
