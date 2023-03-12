/**
 * @file mysqler.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Mar 11 2023
 *
 * This is a header file implementing the mysql connection and query execution
 * It assumes some pre-installment and setup on the localhost's mysql database
 */
#ifndef SRC_INCLUDE_DB_MYSQLER_H_
#define SRC_INCLUDE_DB_MYSQLER_H_

#include <cppconn/driver.h>
#include <cppconn/resultset.h>

#include <future>  // NOLINT
#include <iostream>
#include <memory>
#include <string>

#include "mysql_connection.h"

#include "core/utils.h"

namespace TURTLE_SERVER::DB {

constexpr static char TCP_PROTOCOL[] = "tcp";

/**
 * Each MySqler is a single mysql connection focused on one database for a given server addr + port + user
 * Query execution might throw SQL exception to be handled by caller
 *
 */
class MySqler {
 public:
  /* upon ctor, connect and create a live connection */
  explicit MySqler(const std::string &db_address = "127.0.0.1", int db_port = 3306, const std::string &user = "tester",
                   const std::string &password = "tester", const std::string &db_name = "tester_db");

  ~MySqler() = default;

  NON_COPYABLE(MySqler);

  /* support move */
  MySqler(MySqler &&rhs) noexcept;
  auto operator=(MySqler &&rhs) noexcept -> MySqler &;

  /* blocking query that returns resultset (select) */
  auto ExecuteQueryBlocking(const std::string &command) -> std::unique_ptr<sql::ResultSet>;

  /* non-blocking query that returns resultset (select) */
  auto ExecuteQueryNonBlocking(const std::string &command) -> std::future<std::unique_ptr<sql::ResultSet>>;

  /* blocking true/false command (insert/delete/create/drop/etc) */
  auto ExecuteBlocking(const std::string &command) -> bool;

  /* non-blocking true/false command (insert/delete/create/drop/etc) */
  auto ExecuteNonBlocking(const std::string &command) -> std::future<bool>;

  auto IsValid() -> bool;

 private:
  /* one driver per MySqler lifecycle */
  sql::Driver *driver_;  // no need to delete it
  std::unique_ptr<sql::Connection> conn_;
  bool valid_{false};
};

}  // namespace TURTLE_SERVER::DB

#endif  // SRC_INCLUDE_DB_MYSQLER_H_
