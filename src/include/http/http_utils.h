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

#include <algorithm>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace TURTLE_SERVER::HTTP {

static constexpr char SPACE[] = {" "};
static constexpr char CRLF[] = {"\r\n"};
static constexpr char COLON[] = {":"};
static constexpr char DEFAULT_ROUTE[] = {"index.html"};
static constexpr char HEADER_CONNECTION[] = {"CONNECTION"};
static constexpr char CONNECTION_KEEP_ALIVE[] = {"KEEP-ALIVE"};

/* HTTP Method enum, only support GET method now */
enum class Method { GET, UNSUPPORTED };

/* HTTP version enum, only support HTTP 1.1 now */
enum class Version { HTTP_1_1, UNSUPPORTED };

static const std::map<Method, std::string> METHOD_TO_STRING{
    {Method::GET, "GET"}, {Method::UNSUPPORTED, "UNSUPPORTED"}};

static const std::map<Version, std::string> VERSION_TO_STRING{
    {Version::HTTP_1_1, "HTTP/1.1"}, {Version::UNSUPPORTED, "UNSUPPORTED"}};

/* space and case insensitive */
auto To_Method(std::string method_str) -> Method;

/* space and case insensitive */
auto To_Version(std::string version_str) -> Version;

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

/**
 * convert each character in a string to upper case inplace
 */
void To_Upper(std::string& str);  // NOLINT

}  // namespace TURTLE_SERVER::HTTP

#endif  // SRC_INCLUDE_HTTP_HTTP_UTILS_H_
