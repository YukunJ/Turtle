/**
 * @file utils.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 25 2022
 *
 * This is a header file only helper for some macro definitions
 */

#ifndef SRC_INCLUDE_UTILS_H_
#define SRC_INCLUDE_UTILS_H_

#define NON_COPYABLE(class_name)           \
  class_name(const class_name &) = delete; \
  class_name &operator=(const class_name &) = delete

#define NON_MOVEABLE(class_name)      \
  class_name(class_name &&) = delete; \
  class_name &operator=(class_name &&) = delete

#define NON_COPYABLE_AND_MOVEABLE(class_name)         \
  class_name(const class_name &) = delete;            \
  class_name &operator=(const class_name &) = delete; \
  class_name(class_name &&) = delete;                 \
  class_name &operator=(class_name &&) = delete

#endif  // SRC_INCLUDE_UTILS_H_
