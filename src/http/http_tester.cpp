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
  TURTLE_SERVER::HTTP::Trim(str_3, " ");
  std::cout << "[" << str_3 << "]" << std::endl;

  std::string str_evil = "\r\n\r\n\r\n";
  TURTLE_SERVER::HTTP::Trim(str_evil, TURTLE_SERVER::HTTP::CRLF);
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

  return 0;
}
