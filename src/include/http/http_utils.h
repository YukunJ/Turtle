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

#include <map>
#include <string>
#include <vector>

namespace TURTLE_SERVER::HTTP {

static constexpr char SPACE[] = {" "};
static constexpr char CRLF[] = {"\r\n"};
static constexpr char COLON[] = {":"};
static constexpr char DEFAULT_ROUTE[] = {"index.html"};
static constexpr char HEADER_CONNECTION[] = {"CONNECTION"};
static constexpr char CONNECTION_CLOSE[] = {"CLOSE"};
static constexpr char CONNECTION_KEEP_ALIVE[] = {"KEEP-ALIVE"};

/* Response status */
static constexpr char HEADER_CONTENT_LENGTH[] = {"Content-Length"};
static constexpr char SERVER[] = {"Turtle/1.0"};
static constexpr char TURTLE_HTTP_VERSION[] = {"HTTP/1.1"};
static constexpr char OK[] = {"200 OK"};
static constexpr char BAD_REQUEST[] = {"400 Bad Request"};
static constexpr char NOT_FOUND[] = {"404 Not Found"};
static constexpr char SERVICE_UNAVAILABLE[] = {"503 Service Unavailable"};

/* HTTP Method enum, only support GET method now */
enum class Method { GET, UNSUPPORTED };

/* HTTP version enum, only support HTTP 1.1 now */
enum class Version { HTTP_1_1, UNSUPPORTED };

static const std::map<Method, std::string> METHOD_TO_STRING{
    {Method::GET, "GET"}, {Method::UNSUPPORTED, "UNSUPPORTED"}};

static const std::map<Version, std::string> VERSION_TO_STRING{
    {Version::HTTP_1_1, "HTTP/1.1"}, {Version::UNSUPPORTED, "UNSUPPORTED"}};

/* space and case insensitive */
auto ToMethod(std::string method_str) -> Method;

/* space and case insensitive */
auto ToVersion(std::string version_str) -> Version;

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
 * Convert each character in a string to upper case inplace
 */
void ToUpper(std::string& str);  // NOLINT

/**
 * Check if the path-specified directory exists
 */
bool IsDirectoryExists(const std::string& directory_path);

/**
 * Check if the path-specified path exists
 */
bool IsFileExists(const std::string& file_path);

/**
 * Tell the size of a file in bytes.
 * Assume this file already exists and is verified
 */
size_t CheckFileSize(const std::string& file_path);

/**
 * Load the file into a vector of char
 * able to contain binary data
 */
void LoadFile(const std::string& file_path,
              std::vector<char>& buffer);  // NOLINT

}  // namespace TURTLE_SERVER::HTTP

#endif  // SRC_INCLUDE_HTTP_HTTP_UTILS_H_
