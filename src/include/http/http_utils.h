/**
 * @file http_utils.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 10 2023
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

static constexpr int READ_WRITE_PERMISSION = 0600;

static constexpr char PARAMETER_SEPARATOR[] = {"&"};
static constexpr char UNDERSCORE[] = {"_"};
static constexpr char SPACE[] = {" "};
static constexpr char DOT[] = {"."};
static constexpr char CRLF[] = {"\r\n"};
static constexpr char COLON[] = {":"};
static constexpr char DEFAULT_ROUTE[] = {"index.html"};
static constexpr char CGI_BIN[] = {"cgi-bin"};
static constexpr char CGI_PREFIX[] = {"cgi_temp"};

/* Common Header and Value */
static constexpr char HEADER_SERVER[] = {"Server"};
static constexpr char SERVER_TURTLE[] = {"Turtle/1.0"};
static constexpr char HEADER_CONTENT_LENGTH[] = {"Content-Length"};
static constexpr char HEADER_CONTENT_TYPE[] = {"Content-Type"};
static constexpr char CONTENT_LENGTH_ZERO[] = {"0"};
static constexpr char HEADER_CONNECTION[] = {"Connection"};
static constexpr char CONNECTION_CLOSE[] = {"Close"};
static constexpr char CONNECTION_KEEP_ALIVE[] = {"Keep-Alive"};
static constexpr char HTTP_VERSION_TURTLE[] = {"HTTP/1.1"};

/* MIME Types */
static constexpr char MIME_HTML[] = {"text/html"};
static constexpr char MIME_CSS[] = {"text/css"};
static constexpr char MIME_PNG[] = {"image/png"};
static constexpr char MIME_JPG[] = {"image/jpg"};
static constexpr char MIME_JPEG[] = {"image/jpeg"};
static constexpr char MIME_GIF[] = {"image/gif"};
static constexpr char MIME_OCTET[] = {"application/octet-stream"};

/* Response status */
static constexpr char RESPONSE_OK[] = {"200 OK"};
static constexpr char RESPONSE_BAD_REQUEST[] = {"400 Bad Request"};
static constexpr char RESPONSE_NOT_FOUND[] = {"404 Not Found"};
static constexpr char RESPONSE_SERVICE_UNAVAILABLE[] = {"503 Service Unavailable"};

/* HTTP Method enum, only support GET/HEAD method now */
enum class Method { GET, HEAD, UNSUPPORTED };

/* HTTP version enum, only support HTTP 1.1 now */
enum class Version { HTTP_1_1, UNSUPPORTED };

/* Content Extension enum */
enum class Extension { HTML, CSS, PNG, JPG, JPEG, GIF, OCTET };

static const std::map<Method, std::string> METHOD_TO_STRING{
    {Method::GET, "GET"}, {Method::HEAD, "HEAD"}, {Method::UNSUPPORTED, "UNSUPPORTED"}};

static const std::map<Version, std::string> VERSION_TO_STRING{{Version::HTTP_1_1, "HTTP/1.1"},
                                                              {Version::UNSUPPORTED, "UNSUPPORTED"}};

static const std::map<Extension, std::string> EXTENSION_TO_STRING{
    {Extension::HTML, "HTML"}, {Extension::CSS, "CSS"}, {Extension::PNG, "PNG"},    {Extension::JPG, "JPG"},
    {Extension::JPEG, "JPEG"}, {Extension::GIF, "GIF"}, {Extension::OCTET, "OCTET"}};

/* space and case insensitive */
auto ToMethod(const std::string &method_str) noexcept -> Method;

/* space and case insensitive */
auto ToVersion(const std::string &version_str) noexcept -> Version;

/* space and case insensitive */
auto ToExtension(const std::string &extension_str) noexcept -> Extension;

/* space and case insensitive */
auto ExtensionToMime(const Extension &extension) noexcept -> std::string;

/**
 * split a string into many sub strings, splitted by the specified delimiter
 */
auto Split(const std::string &str, const char *delim = SPACE) noexcept -> std::vector<std::string>;

/**
 * concatenate a collection of strings using the specified delimiter
 */
auto Join(const std::vector<std::string> &tokens, const char *delim = SPACE) noexcept -> std::string;

/**
 * Remove the leading and trailing specified delimiter (not inplace)
 */
auto Trim(const std::string &str, const char *delim = SPACE) noexcept -> std::string;

/**
 * Convert each character in a string to upper case (not inplace)
 */
auto ToUpper(std::string str) noexcept -> std::string;

/**
 * Apply Trim + ToUpper to a string and return the formatted version of it
 */
auto Format(const std::string &) noexcept -> std::string;

/**
 * Check if the path-specified directory exists
 */
auto IsDirectoryExists(const std::string &directory_path) noexcept -> bool;

/**
 * Inspect if the request is dynamic CGI
 * hardcode check if contains the cgi-bin folder in resource url path
 */
auto IsCgiRequest(const std::string &resource_url) noexcept -> bool;

/**
 * Check if the path-specified path exists
 */
auto IsFileExists(const std::string &file_path) noexcept -> bool;

/**
 * Delete a file if exists
 */
auto DeleteFile(const std::string &file_path) noexcept -> bool;

/**
 * Tell the size of a file in bytes.
 * Assume this file already exists and is verified
 */
auto CheckFileSize(const std::string &file_path) noexcept -> size_t;

/**
 * Load the file appending to be back of a vector of unsigned char
 * able to contain binary data
 */
void LoadFile(const std::string &file_path,
              std::vector<unsigned char> &buffer) noexcept;  // NOLINT

}  // namespace TURTLE_SERVER::HTTP

#endif  // SRC_INCLUDE_HTTP_HTTP_UTILS_H_
