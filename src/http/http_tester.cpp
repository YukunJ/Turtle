/**
 * @file http_tester.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 29 2022
 *
 * This is an mini tester file for various http module functionality
 */

#include "http/header.h"
#include "http/http_utils.h"
#include "http/request.h"
#include "http/response.h"

template <typename T>
void print_vector(const std::vector<T>& vec) {
  std::cout << "vector contains: " << std::endl;
  std::cout << "BEGIN[" << std::endl;
  for (auto& e : vec) {
    std::cout << e << std::endl;
  }
  std::cout << "]END" << std::endl;
}

int main() {
  /* Split */
  std::string str1("There are two needles in this haystack with needles. ");
  auto split_str1 = TURTLE_SERVER::HTTP::Split(str1, " ");
  print_vector(split_str1);

  std::string str2(
      "Get /mysite/index.html HTTP/1.1\r\nHost: 10.101.101.10\r\nAccept: "
      "*/*\r\n\r\n");
  auto split_str2 = TURTLE_SERVER::HTTP::Split(str2, TURTLE_SERVER::HTTP::CRLF);
  print_vector(split_str2);

  /* Join */
  std::vector<std::string> collection1 = {"Hello Yo", "This is Jason",
                                          "Not bad is it?"};
  std::cout << TURTLE_SERVER::HTTP::Join(collection1, "!") << std::endl;

  if (TURTLE_SERVER::HTTP::Join(split_str2, TURTLE_SERVER::HTTP::CRLF) ==
      str2) {
    std::cout << ".split().join() equals itself" << std::endl;
  }

  /* Trim */
  std::string str_3 = "  Sometimes Life is just so hard! ";
  str_3 = TURTLE_SERVER::HTTP::Trim(str_3, " ");
  std::cout << "[" << str_3 << "]" << std::endl;

  std::string str_evil = "\r\n\r\n\r\n";
  str_evil = TURTLE_SERVER::HTTP::Trim(str_evil, TURTLE_SERVER::HTTP::CRLF);
  std::cout << "[" << str_evil << "]" << std::endl;

  /* Header */
  TURTLE_SERVER::HTTP::Header header1{"Content-Type",
                                      "application/x-www-form-urlencoded"};
  std::cout << header1 << std::endl;

  std::string line2{"Content-Type: application/x-www-form-urlencoded"};
  TURTLE_SERVER::HTTP::Header header2{line2};
  std::cout << header2 << std::endl;

  std::string bad_line_1{"Content-Type"};
  TURTLE_SERVER::HTTP::Header header3{bad_line_1};
  std::cout << header3 << std::endl;

  std::string bad_line_2{"Content-Type: Something: Something else"};
  TURTLE_SERVER::HTTP::Header header4{bad_line_2};
  std::cout << header4 << std::endl;

  std::string bad_line_3{":"};
  TURTLE_SERVER::HTTP::Header header5{bad_line_3};
  std::cout << header5 << std::endl;

  std::string problem_line1{" Get "};
  TURTLE_SERVER::HTTP::Trim(problem_line1);
  TURTLE_SERVER::HTTP::ToUpper(problem_line1);
  std::cout << problem_line1 << std::endl;
  if (TURTLE_SERVER::HTTP::ToMethod(problem_line1) ==
      TURTLE_SERVER::HTTP::Method::GET) {
    std::cout << "Parse to Get Method\n" << std::endl;
  }

  std::string problem_line2{" Http/1.1 "};
  TURTLE_SERVER::HTTP::Trim(problem_line2);
  TURTLE_SERVER::HTTP::ToUpper(problem_line2);
  std::cout << problem_line2 << std::endl;
  if (TURTLE_SERVER::HTTP::ToVersion(problem_line2) ==
      TURTLE_SERVER::HTTP::Version::HTTP_1_1) {
    std::cout << "Parse to HTTP/1.1 version\n" << std::endl;
  }

  /* not end with /r/n */
  std::string request_0_str =
      "GET /hello.html HTTP/1.1\r\n"
      "User-Agent: Mozilla/4.0\r\n"
      "Host: www.tutorialspoint.com\r\n"
      "Connection: Keep-Alive\r\n";
  TURTLE_SERVER::HTTP::Request request_0{request_0_str};
  std::cout << request_0 << std::endl;

  /* first line is not request line */
  std::string request_1_str =
      "User-Agent: Mozilla/4.0\r\n"
      "GET /hello.html HTTP/1.1\r\n"
      "Host: www.tutorialspoint.com\r\n"
      "Connection: Keep-Alive\r\n"
      "\r\n";
  TURTLE_SERVER::HTTP::Request request_1{request_1_str};
  std::cout << request_1 << std::endl;

  /* method not supported */
  std::string request_2_str =
      "HEAD /hello.html HTTP/1.1\r\n"
      "User-Agent: Mozilla/4.0\r\n"
      "Host: www.tutorialspoint.com\r\n"
      "Connection: Keep-Alive\r\n"
      "\r\n";
  TURTLE_SERVER::HTTP::Request request_2{request_2_str};
  std::cout << request_2 << std::endl;

  /* version not supported */
  std::string request_3_str =
      "GET /hello.html HTTP/2.0\r\n"
      "User-Agent: Mozilla/4.0\r\n"
      "Host: www.tutorialspoint.com\r\n"
      "Connection: Keep-Alive\r\n"
      "\r\n";
  TURTLE_SERVER::HTTP::Request request_3{request_3_str};
  std::cout << request_3 << std::endl;

  /* the bare minimum valid request */
  std::string request_4_str =
      "GET /hello.html HTTP/1.1\r\n"
      "\r\n";
  TURTLE_SERVER::HTTP::Request request_4{request_4_str};
  std::cout << request_4 << std::endl;

  /* the default index.html routing request */
  std::string request_5_str =
      "GET some_folder/ HTTP/1.1\r\n"
      "\r\n";
  TURTLE_SERVER::HTTP::Request request_5{request_5_str};
  std::cout << request_5 << std::endl;

  /* connection to close request */
  std::string request_6_str =
      "GET /hello.html HTTP/1.1\r\n"
      "User-Agent: Mozilla/4.0\r\n"
      "Host: www.tutorialspoint.com\r\n"
      "Connection: close\r\n"
      "\r\n";
  TURTLE_SERVER::HTTP::Request request_6{request_6_str};
  std::cout << request_6 << std::endl;

  /* connection to kepp alive request */
  std::string request_7_str =
      "GET /hello.html HTTP/1.1\r\n"
      "User-Agent: Mozilla/4.0\r\n"
      "Host: www.tutorialspoint.com\r\n"
      "Connection: Keep-Alive\r\n"
      "\r\n";
  TURTLE_SERVER::HTTP::Request request_7{request_7_str};
  std::cout << request_7 << std::endl;

  std::string file_name = "./hello.txt";
  bool exists = TURTLE_SERVER::HTTP::IsFileExists(file_name);
  if (exists) {
    std::cout << "successfully locate file " << file_name << std::endl;
    std::cout << "size is " << TURTLE_SERVER::HTTP::CheckFileSize(file_name)
              << std::endl;
  } else {
    std::cout << file_name << " does not exist" << std::endl;
  }
  std::string directory_path = "./my_dir";
  bool dir_exists = TURTLE_SERVER::HTTP::IsDirectoryExists(directory_path);
  if (dir_exists) {
    std::cout << "successfully locate directory " << directory_path
              << std::endl;
  } else {
    std::cout << directory_path << " directory does not exist" << std::endl;
  }

  std::vector<unsigned char> response_buf;
  auto response_400 = TURTLE_SERVER::HTTP::Response::Make400Response();
  response_400.Serialize(response_buf);
  std::cout << "The 400 response is as follows: " << std::endl;
  std::cout << std::string(response_buf.begin(), response_buf.end())
            << std::endl;
  response_buf.clear();

  auto response_404 = TURTLE_SERVER::HTTP::Response::Make404Response();
  response_404.Serialize(response_buf);
  std::cout << "The 404 response is as follows: " << std::endl;
  std::cout << std::string(response_buf.begin(), response_buf.end())
            << std::endl;
  response_buf.clear();

  auto response_503 = TURTLE_SERVER::HTTP::Response::Make503Response();
  response_503.Serialize(response_buf);
  std::cout << "The 503 response is as follows: " << std::endl;
  std::cout << std::string(response_buf.begin(), response_buf.end())
            << std::endl;
  response_buf.clear();

  auto response_200_close =
      TURTLE_SERVER::HTTP::Response::Make200Response(true, "./hello.txt");
  response_200_close.Serialize(response_buf);
  std::cout << "The 200 close response is as follows: " << std::endl;
  std::cout << std::string(response_buf.begin(), response_buf.end())
            << std::endl;
  response_buf.clear();

  auto response_200_alive =
      TURTLE_SERVER::HTTP::Response::Make200Response(false, "./hello.txt");
  response_200_alive.Serialize(response_buf);
  std::cout << "The 200 keep-alive response is as follows: " << std::endl;
  std::cout << std::string(response_buf.begin(), response_buf.end())
            << std::endl;
  response_buf.clear();
  return 0;
}
