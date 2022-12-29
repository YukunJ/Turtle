/**
 * @file http_utils.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 29 2022
 *
 * This is a header file for http module's constant enum definitions and utility
 * functions
 */

#ifndef SRC_INCLUDE_HTTP_HTTP_UTILS_H_
#define SRC_INCLUDE_HTTP_HTTP_UTILS_H_

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace TURTLE_SERVER::HTTP {

static constexpr char SPACE[] = {" "};
static constexpr char CRLF[] = {"\r\n"};
static constexpr char COLON[] = {":"};

/**
 * split a string into many sub strings, splitted by the specified delimiter
 */
auto Split(const std::string& str, const char* delim = SPACE)
    -> std::vector<std::string>;

/**
 * concatenate a collection of strings using the specified delimiter
 */
auto Join(const std::vector<std::string>& tokens, const char* delim = SPACE)
    -> std::string;

/**
 * Remove the leading and trailing specified delimiter inplace
 */
void Trim(std::string& str, const char* delim = SPACE);  // NOLINT

}  // namespace TURTLE_SERVER::HTTP

#endif  // SRC_INCLUDE_HTTP_HTTP_UTILS_H_
