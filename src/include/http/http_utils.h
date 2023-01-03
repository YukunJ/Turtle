/**
 * @file http_utils.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 3 2023
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

/* Common Header and Value */
static constexpr char HEADER_SERVER[] = {"Server"};
static constexpr char SERVER_TURTLE[] = {"Turtle/1.0"};
static constexpr char HEADER_CONTENT_LENGTH[] = {"Content-Length"};
static constexpr char CONTENT_LENGTH_ZERO[] = {"0"};
static constexpr char HEADER_CONNECTION[] = {"Connection"};
static constexpr char CONNECTION_CLOSE[] = {"Close"};
static constexpr char CONNECTION_KEEP_ALIVE[] = {"Keep-Alive"};
static constexpr char HTTP_VERSION_TURTLE[] = {"HTTP/1.1"};

/* Response status */
static constexpr char RESPONSE_OK[] = {"200 OK"};
static constexpr char RESPONSE_BAD_REQUEST[] = {"400 Bad Request"};
static constexpr char RESPONSE_NOT_FOUND[] = {"404 Not Found"};
static constexpr char RESPONSE_SERVICE_UNAVAILABLE[] = {
    "503 Service Unavailable"};

/* HTTP Method enum, only support GET/HEAD method now */
enum class Method { GET, HEAD, UNSUPPORTED };

/* HTTP version enum, only support HTTP 1.1 now */
enum class Version { HTTP_1_1, UNSUPPORTED };

static const std::map<Method, std::string> METHOD_TO_STRING{
    {Method::GET, "GET"},
    {Method::HEAD, "HEAD"},
    {Method::UNSUPPORTED, "UNSUPPORTED"}};

static const std::map<Version, std::string> VERSION_TO_STRING{
    {Version::HTTP_1_1, "HTTP/1.1"}, {Version::UNSUPPORTED, "UNSUPPORTED"}};

/* space and case insensitive */
auto ToMethod(const std::string& method_str) noexcept -> Method;

/* space and case insensitive */
auto ToVersion(const std::string& version_str) noexcept -> Version;

/**
 * split a string into many sub strings, splitted by the specified delimiter
 */
auto Split(const std::string& str, const char* delim = SPACE) noexcept
    -> std::vector<std::string>;

/**
 * concatenate a collection of strings using the specified delimiter
 */
auto Join(const std::vector<std::string>& tokens,
          const char* delim = SPACE) noexcept -> std::string;

/**
 * Remove the leading and trailing specified delimiter (not inplace)
 */
auto Trim(const std::string& str, const char* delim = SPACE) noexcept
    -> std::string;

/**
 * Convert each character in a string to upper case (not inplace)
 */
auto ToUpper(std::string str) noexcept -> std::string;

/**
 * Apply Trim + ToUpper to a string and return the formatted version of it
 */
auto Format(const std::string&) noexcept -> std::string;

/**
 * Check if the path-specified directory exists
 */
auto IsDirectoryExists(const std::string& directory_path) noexcept -> bool;

/**
 * Check if the path-specified path exists
 */
auto IsFileExists(const std::string& file_path) noexcept -> bool;

/**
 * Tell the size of a file in bytes.
 * Assume this file already exists and is verified
 */
auto CheckFileSize(const std::string& file_path) noexcept -> size_t;

/**
 * Load the file appending to be back of a vector of unsigned char
 * able to contain binary data
 */
void LoadFile(const std::string& file_path,
              std::vector<unsigned char>& buffer) noexcept;  // NOLINT

}  // namespace TURTLE_SERVER::HTTP

#endif  // SRC_INCLUDE_HTTP_HTTP_UTILS_H_
